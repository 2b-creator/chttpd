int make_db();

char random_char();
void generate_uuid(char *uuid);
char* remove_quotes(const char *str);
int auth(const char *token);
int get_userid(const char *token);
int rec_send_report(int userid, int report_type, const char *description, const char *uuid_repo);