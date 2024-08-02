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

struct ConnectionInfo {
    char *upload_data;
    size_t upload_data_size;
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
    struct ConnectionInfo *con_info = *con_cls;
    
    if (strcmp(method, "POST") == 0)
    {
        if (!con_info)
        {
            con_info = malloc(sizeof(struct ConnectionInfo));
            if (!con_info)
                return MHD_NO;
            
            con_info->upload_data = NULL;
            con_info->upload_data_size = 0;
            *con_cls = con_info;
            return MHD_YES;
        }

        if (*upload_data_size != 0)
        {
            con_info->upload_data = realloc(con_info->upload_data, con_info->upload_data_size + *upload_data_size + 1);
            if (!con_info->upload_data)
                return MHD_NO;
            
            memcpy(con_info->upload_data + con_info->upload_data_size, upload_data, *upload_data_size);
            con_info->upload_data_size += *upload_data_size;
            con_info->upload_data[con_info->upload_data_size] = '\0';
            *upload_data_size = 0;
            return MHD_YES;
        }
        else
        {
            if (con_info->upload_data_size == 0)
                return MHD_NO;

            printf("URL: %s\n", url);
            if (strcmp(url, "/api/user/send") == 0)
            {
                printf("Received Data: %s\n", con_info->upload_data);

                cJSON *json = cJSON_Parse(con_info->upload_data);
                if (json == NULL)
                {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL)
                    {
                        fprintf(stderr, "Error before: %s\n", error_ptr);
                    }
                    free(con_info->upload_data);
                    free(con_info);
                    return MHD_NO;
                }

                cJSON *reportTypeRaw = cJSON_GetObjectItem(json, "report_type");
                if (!cJSON_IsNumber(reportTypeRaw))
                {
                    fprintf(stderr, "report_type is not a number\n");
                    cJSON_Delete(json);
                    free(con_info->upload_data);
                    free(con_info);
                    return MHD_NO;
                }
                int report_type = reportTypeRaw->valueint;

                cJSON *descriptionRaw = cJSON_GetObjectItemCaseSensitive(json, "description");
                if (!cJSON_IsString(descriptionRaw))
                {
                    fprintf(stderr, "description is not a string\n");
                    cJSON_Delete(json);
                    free(con_info->upload_data);
                    free(con_info);
                    return MHD_NO;
                }
                char *description = descriptionRaw->valuestring;

                const char *auth_header = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Authorization");
                int userid = get_userid(auth_header);
                if (userid == 0)
                {
                    cJSON_Delete(json);
                    free(con_info->upload_data);
                    free(con_info);
                    const char *error_response = "{\"error\": \"Unauthorized\"}";
                    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_response), (void *)error_response, MHD_RESPMEM_PERSISTENT);
                    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");
                    int ret = MHD_queue_response(connection, MHD_HTTP_UNAUTHORIZED, response);
                    MHD_destroy_response(response);
                    return ret;
                }

                char uuid_repo[37];
                generate_uuid(uuid_repo);
                int isSuccess = rec_send_report(userid, report_type, description, auth_header);
                char *respRaw = NULL;
                if (!isSuccess)
                {
                    cJSON *resp_data = cJSON_CreateObject();
                    cJSON_AddNumberToObject(resp_data, "code", 200);
                    cJSON_AddStringToObject(resp_data, "message", "report send successfully");
                    respRaw = cJSON_Print(resp_data);
                    cJSON_Delete(resp_data);
                }
                else
                {
                    respRaw = strdup("{\"code\":500, \"message\":\"report send failed\"}");
                }

                struct MHD_Response *response = MHD_create_response_from_buffer(strlen(respRaw), (void *)respRaw, MHD_RESPMEM_MUST_COPY);
                MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");
                if (response == NULL)
                {
                    cJSON_Delete(json);
                    free(respRaw);
                    free(con_info->upload_data);
                    free(con_info);
                    return MHD_NO;
                }
                int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
                MHD_destroy_response(response);
                free(respRaw);
                cJSON_Delete(json);
                free(con_info->upload_data);
                free(con_info);
                return ret;
            }
        }
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

    // make_db();

    printf("Server is running on port %d\n", PORT);
    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}
// gpt is good