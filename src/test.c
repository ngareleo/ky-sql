#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utilities/utilities.h"

void TestStrTok()
{
    char *delim = ",";
    const char *target = "'The Great, Gatsby','F. \\'Scott\\' ,Fitzgerald','9780743273565','Fiction',1925,14.99,18,2.25";
    char *fn;

    if ((fn = malloc(strlen(target) + 1)))
    {
        strcpy(fn, target);
        char *token = strtok(fn, delim);

        while (token)
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
    TestStrTok();
    TestCount();
    return 0;
}