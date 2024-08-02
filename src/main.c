#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <cJSON.h>
#include "db_fun.h"

#define PORT 8888

const char *end_points_post[] = {
    "/api/user/send",
};

const char *end_points_get[] = {
    "/api/user/get",
};

static int authenticate_request(struct MHD_Connection *connection)
{
    const char *auth_header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Authorization");
    if (auth(auth_header))
    {
        return 1;
    }
    return 0;
}

static int request_handler(void *cls,
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
        if (*upload_data_size != 0)
        {
            printf("%s", url);
            if (strcmp(url, "/api/user/send") == 0)
            {
                char *upload_data_copy = malloc(*upload_data_size + 1);
                memcpy(upload_data_copy, upload_data, *upload_data_size);
                upload_data_copy[*upload_data_size] = '\0';
                printf("%s\n", upload_data_copy);

                cJSON *json = cJSON_Parse(upload_data_copy);
                if (json == NULL)
                {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL)
                    {
                        fprintf(stderr, "Error before: %s\n", error_ptr);
                    }
                    return MHD_NO;
                }
                cJSON *reportTypeRaw = cJSON_GetObjectItem(json, "report_type");
                int report_type = reportTypeRaw->valueint;
                cJSON *descriptionRaw = cJSON_GetObjectItemCaseSensitive(json, "description");
                char *description = descriptionRaw->valuestring;
                const char *auth_header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Authorization");
                int userid = get_userid(auth_header);
                if (userid == 0)
                {
                    return MHD_NO;
                }
                char *respRaw = "null";
                char uuid_repo[37];
                generate_uuid(uuid_repo);
                int isSuccess = rec_send_report(userid, report_type, description, auth_header);
                if (isSuccess)
                {
                    cJSON *resp_data = cJSON_CreateObject();
                    cJSON_AddNumberToObject(resp_data,"code",200);
                    cJSON_AddStringToObject(resp_data,"message","report send successfully");
                    respRaw = cJSON_Print(resp_data);
                    cJSON_Delete(resp_data);
                }
                struct MHD_Response *response = MHD_create_response_from_buffer(strlen(respRaw),(void *)respRaw,MHD_RESPMEM_MUST_COPY);
                MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");
                int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
                MHD_destroy_response(response);
                free(respRaw);
                free(upload_data_copy);
                return ret;
            }
        }
        return MHD_YES;
    }
    else if (strcmp(method, "GET") == 0)
    {
        return MHD_YES;
    }

    return MHD_NO;
}

int main()
{
    system("chcp 65001 & cls");
    setlocale(LC_ALL, "");
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &request_handler, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    //make_db();

    printf("Server is running on port %d\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
// ver