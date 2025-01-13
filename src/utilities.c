// Utility functions that are decoupled from the application

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE ' '
#define SEMICOLON ';'

///
/// Free multi-dimensional array
///
void multifree(void **arr, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        free(arr[i]);
    }

    free(arr);
}

int counttokens(char *text, char match)
{
    size_t n = strlen(text);
    if (n <= 0)
        return -1;

    int nt, skip = 0;
    for (int i = 0; i < n; i++)
    {
        if (*(text + i) == match)
        {
            if (!skip)
            {
                nt++;
                skip = 1;
            }
        }
        else
        {
            skip = 0;
        }
    }

    return nt;
}

///
///  Assumes there's a semicolon
///
char **tokenize(char *text)
{
    char **out;
    int ns = 0,
        skip = 0,
        count = 0,
        num = counttokens(text, SPACE);

    if (num < 0)
        return NULL;

    for (const char *marker = text, *chars = text; count < num + 1; chars++)
    {
        if (*chars == SPACE || *chars == SEMICOLON)
        {

            if (skip)
            {
                continue;
            }

            int width = chars - marker; // word width

            out[count] = (char *)malloc(sizeof(char) * width);

            if (out[count] == NULL)
            {
                multifree((void **)out, count);
                return NULL;
            }

            strncpy(out[count], marker, width);

            out[count][width] = '\0';

            count++;
            marker = chars + 1;
            skip = 1; // indicate that we have hit some space
        }
        else
        {
            skip = 0;
        }
    }

    out[count] = NULL;
    return out;
}
