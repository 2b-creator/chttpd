#include <libpq-fe.h>
#include "db_fun.h"
const char *conninfo = "dbname = repo_db user = repo_admin password = repo_admin hostaddr = 127.0.0.1 port = 5432";

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
    printf("success!");

    char *create_users_table = "CREATE TABLE Users (\n"
                               "UserID INT PRIMARY KEY AUTO_INCREMENT,\n"
                               "Username VARCHAR(50) UNIQUE NOT NULL,\n"
                               "PasswordHash VARCHAR(255) NOT NULL,\n"
                               "Email VARCHAR(100) UNIQUE NOT NULL,\n"
                               "CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
                               "IsAdmin BOOLEAN DEFAULT FALSE);";

    char *create_report_types = "CREATE TABLE Reports (\n"
                                "ReportID INT PRIMARY KEY AUTO_INCREMENT,\n"
                                "UserID INT,\n"
                                "ReportType INT,\n"
                                "Description TEXT NOT NULL,\n"
                                "Status ENUM('Pending', 'Resolved', 'Rejected') DEFAULT 'Pending',\n"
                                "CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
                                "UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\n"
                                "FOREIGN KEY (UserID) REFERENCES Users(UserID),\n"
                                "FOREIGN KEY (ReportType) REFERENCES ReportTypes(TypeID)\n"
                                ");\n";

    char *create_report_tables = "CREATE TABLE Reports (\n"
                                 "ReportID INT PRIMARY KEY AUTO_INCREMENT,\n"
                                 "UserID INT,\n"
                                 "ReportType INT,\n"
                                 "Description TEXT NOT NULL,\n"
                                 "Status ENUM('Pending', 'Resolved', 'Rejected') DEFAULT 'Pending',\n"
                                 "CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
                                 "UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,\n"
                                 "FOREIGN KEY (UserID) REFERENCES Users(UserID),\n"
                                 "FOREIGN KEY (ReportType) REFERENCES ReportTypes(TypeID)\n"
                                 ");";

    char *create_report_actions = "CREATE TABLE ReportActions (\n"
                                  "ActionID INT PRIMARY KEY AUTO_INCREMENT,\n"
                                  "ReportID INT,\n"
                                  "AdminUserID INT,\n"
                                  "ActionTaken TEXT NOT NULL,\n"
                                  "ActionDate TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n"
                                  "FOREIGN KEY (ReportID) REFERENCES Reports(ReportID),\n"
                                  "FOREIGN KEY (AdminUserID) REFERENCES Users(UserID)\n"
                                  ");";

    char *create_tables[] = {create_users_table, create_report_types, create_report_tables, create_report_actions};
    for (size_t i = 0; i < 4; i++)
    {
        res = PQexec(conn, create_tables[i]);
        if (PQresultStatus(res) != PGRES_TUPLES_OK)
        {
            fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
            PQclear(res);
            exit_nicely(conn);
        }
    }

    return 0;
}