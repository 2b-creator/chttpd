#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libpq-fe.h>
#include <toml.h>
#include "db_fun.h"

const char *conninfo = "dbname = repo_db user = repo_admin password = repo_admin hostaddr = 127.0.0.1 port = 5432 options='--client_encoding=UTF8'";

void exit_nicely(PGconn *conn);

void exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}
int make_db()
{
    PGconn *conn = PQconnectdb(conninfo);
    PGresult *res;
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        exit_nicely(conn);
    }
    printf("success!\n");

    char *create_users_table = "CREATE TABLE Users (\n"
                               "UserID SERIAL PRIMARY KEY,\n"
                               "Username VARCHAR(50) UNIQUE NOT NULL,\n"
                               "PasswordHash VARCHAR(255) NOT NULL,\n"
                               "CreatedAt TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,\n"
                               "UUID VARCHAR(100) UNIQUE NOT NULL,\n"
                               "IsAdmin BOOLEAN DEFAULT FALSE\n"
                               ");";

    char *create_report_types = "CREATE TABLE ReportTypes (\n"
                                "TypeID SERIAL PRIMARY KEY,\n"
                                "TypeName VARCHAR(50) UNIQUE NOT NULL\n"
                                ");";

    char *create_report_tables = "CREATE TABLE Reports (\n"
                                 "ReportID SERIAL PRIMARY KEY,\n"
                                 "UserID INT REFERENCES Users(UserID),\n"
                                 "ReportType INT REFERENCES ReportTypes(TypeID),\n"
                                 "Description TEXT NOT NULL,\n"
                                 "Status VARCHAR(20) DEFAULT 'Pending',\n"
                                 "CreatedAt TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,\n"
                                 "UpdatedAt TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP\n"
                                 ");";

    char *create_report_actions = "CREATE TABLE ReportActions (\n"
                                  "ActionID SERIAL PRIMARY KEY,\n"
                                  "ReportID INT REFERENCES Reports(ReportID),\n"
                                  "AdminUserID INT REFERENCES Users(UserID),\n"
                                  "ActionTaken TEXT NOT NULL,\n"
                                  "ActionDate TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP\n"
                                  ");";

    char *create_tables[] = {create_users_table, create_report_types, create_report_tables, create_report_actions};
    for (size_t i = 0; i < 4; i++)
    {
        res = PQexec(conn, create_tables[i]);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            fprintf(stderr, "CREATE failed: %s", PQerrorMessage(conn));
            PQclear(res);
            exit_nicely(conn);
        }
        PQclear(res);
    }

    FILE *configFile;
    configFile = fopen("./src/config/config.toml", "r");
    if (!configFile)
    {
        perror("file error that");
        return 1;
    }
    toml_table_t *config = toml_parse_file(configFile, NULL, 0);
    if (!config)
    {
        fprintf(stderr, "file error\n");
        fclose(configFile);
        return 1;
    }
    toml_table_t *admin = toml_table_in(config, "admin");
    const char *username;
    const char *password;
    if (admin)
    {
        username = toml_raw_in(admin, "username");
        password = toml_raw_in(admin, "password");
    }

    // insert root accout
    srand((unsigned int)time(NULL));

    char uuid[37];
    generate_uuid(uuid);

    const char *paramValues[] = {username, password, uuid};
    const int paramLength[] = {0, 0, 0};

    res = PQexecParams(conn, "INSERT INTO Users (Username, PasswordHash, UUID) VALUES ($1, $2, $3)", 3, NULL, paramValues, paramLength, paramLength, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "INSERT command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }
    toml_free(config);
    fclose(configFile);
    PQfinish(conn);

    return 0;
}