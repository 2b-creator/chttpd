#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <cJSON.h>
#include "db_fun.h"

#define PORT 8888

const char *end_points[] = {"/api/"};

int request_handler(void *cls,
                    struct MHD_Connection *connection,
                    const char *url,
                    const char *method,
                    const char *version,
                    const char *upload_data,
                    size_t *upload_data_size,
                    void **con_cls)
{
    if (strcmp(method, "POST") == 0)
    {
        return MHD_YES;
    }

    return MHD_NO;
}

int main()
{
    system("chcp 65001 & cls");
    printf("你好，世界！\n");
    setlocale(LC_ALL, "");
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &request_handler, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;
    
    make_db();

    printf("Server is running on port %d\n", PORT);
    getchar();
    
    MHD_stop_daemon(daemon);
    return 0;
}