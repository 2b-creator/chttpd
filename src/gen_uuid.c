#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "db_fun.h"
char random_char() {
    const char *chars = "0123456789abcdef";
    return chars[rand() % 16];
}

void generate_uuid(char *uuid) {
    const int UUID_LENGTH = 36;
    int i;

    for (i = 0; i < UUID_LENGTH; i++) {
        switch (i) {
            case 8:
            case 13:
            case 18:
            case 23:
                uuid[i] = '-';
                break;
            case 14:
                uuid[i] = '4';
                break;
            default:
                uuid[i] = random_char();
        }
    }
    uuid[UUID_LENGTH] = '\0';
}
