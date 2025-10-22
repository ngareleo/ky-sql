#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utilities/utilities.h"

void TestStrTok()
{

    char *delim = "\r";
    const char *target = "ready\rpla/\\ryer\rone";
    char *fn;

    if (malloc(strlen(target) + 1))
    {
        strcpy(fn, target);
        char *token = strtok(fn, delim);

        while (token != NULL)
        {
            fprintf(stdout, "Token:  %s\n", token);
            token = strtok(NULL, delim);
        }
    }
}

void TestCount()
{
    char *names[] = {
        "Leo",
        "Kems",
        "Bishana",
        NULL,
    };

    fprintf(stdout, "name count: %d\n", Count((void **)names));
}

int main()
{
    TestCount();
    return 0;
}