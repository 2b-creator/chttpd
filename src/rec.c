#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>
#include <time.h>
#include <libpq-fe.h>
#include "db_fun.h"

int rec_send_report(int userid, int report_type, const char *description, const char *uuid_repo)
{
    FILE *configFile;
    configFile = fopen("./src/config/config.toml", "r");
    if (!configFile)
    {
        perror("file error that");
        return 1;
    }
    char useridRaw[10];
    char report_typeRaw[10];
    sprintf(report_typeRaw, "%d", report_type);
    sprintf(useridRaw, "%d", userid);
    toml_table_t *config = toml_parse_file(configFile, NULL, 0);
    if (!config)
    {
        fprintf(stderr, "file error\n");
        fclose(configFile);
        return 1;
    }
    toml_table_t *database = toml_table_in(config, "database");
    const char *dbname_raw = toml_raw_in(database, "name");
    const char *user_raw = toml_raw_in(database, "user");
    const char *dbpassword_raw = toml_raw_in(database, "password");
    const char *hostaddr_raw = toml_raw_in(database, "server");

    char *dbname = remove_quotes(dbname_raw);
    char *user = remove_quotes(user_raw);
    char *dbpassword = remove_quotes(dbpassword_raw);
    char *hostaddr = remove_quotes(hostaddr_raw);

    int port;
    sscanf(remove_quotes(toml_raw_in(database, "port")), "%d", &port);
    char conn_string[512];
    snprintf(conn_string, sizeof(conn_string), "dbname=%s user=%s password=%s hostaddr=%s port=%d options='--client_encoding=UTF8'", dbname, user, dbpassword, hostaddr, port);

    PGconn *conn = PQconnectdb(conn_string);
    PGresult *res;
    const char *paramValues[] = {
        useridRaw,
        report_typeRaw,
        description,
        uuid_repo,
    };
    const int paramLength[] = {0, 0, 0, 0};
    res = PQexecParams(conn, "INSERT INTO reports (userid, reporttype, description, uuid) VALUES ($1, $2, $3, $4);", 4, NULL, paramValues, paramLength, paramLength, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
        return 0;
    }
    sscanf(useridRaw, "%d", &userid);
    toml_free(config);
    fclose(configFile);
    PQfinish(conn);
    return 1;
}