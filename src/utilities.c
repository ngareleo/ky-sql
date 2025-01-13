// Utility functions that are decoupled from the application

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE ' '
#define SEMICOLON ';'

/**
 * Free multi-dimensional array
 */
void multifree(void **arr, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        free(arr[i]);
    }

    free(arr);
}

///
///  Assumes there's a semicolon
///
char **tokenize(char *text)
{
    size_t n;

    if (!(n = strlen(text)))
        return NULL;

    if (*(text + strlen(text) - 1) != SEMICOLON)
        return NULL;

    // Step 1. count number of tokens

    char **out;
    int ns = 0, skip = 0;

    for (int i = 0; i < n; i++)
    {
        if (*(text + i) == ' ')
        {
            // check if we previously hit a space
            if (skip)
            {
                continue;
            }

            ns++;
            skip = 1; // we hit some white space
        }
        else
        {
            skip = 0;
        }
    }

    out = (char **)malloc(sizeof(char *) * ns + 1);
    if (!out)
        return NULL;

    skip = 0;

    int count = 0;

    for (const char *marker = text, *chars = text; count < ns + 1; chars++)
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
