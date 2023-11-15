#define LOG_CPP

#include <stdio.h>
#include <stdlib.h>

#include "../include/log.h"

FILE *LOG_FILE = open_log();

FILE *open_log(void)
{
    FILE *file = fopen("log.log", "wb");
    if(file == NULL)
    {
        fprintf(stderr, "Can`t open log-file.\n"
                        "Using stderr insead.\n");

        return stderr;
    }

    atexit(close_log);

    setbuf(file, NULL);

    return file;
}

void close_log(void)
{
    fflush(LOG_FILE);

    fclose(LOG_FILE);
}

#undef LOG_CPP