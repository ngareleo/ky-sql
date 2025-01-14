// Utility functions that are decoupled from the application

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE ' '
#define SEMICOLON ';'

typedef char *Token;
typedef char **Tokens;

///
/// Free multi-dimensional array
///
void free_tokens(Tokens tokens, size_t size)
{
    for (int i = 0; i < size; i++)
        free(tokens[i]);

    free(tokens);
}

int count_tokens_flat_list(Tokens tokens)
{
    int count = 0;
    for (char *token = *tokens; token != NULL; count++)
        ;
    return count;
}

int strip(char *text)
{
    char *ptext;
    int len = strlen(text);

    if (len)
    {
        return 0;
    }

    for (ptext = text; ptext - text < len && *ptext == SPACE; ptext++)
    {
        printf("C %c\n", *ptext);
    }

    text = ptext;
    return 0;
}

int count_tokens_from_text(char *text, char match)
{
    if (!strip(text))
        return -1;

    printf("Within count tokens: Text %s\n", text);
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
    int ns = 0,
        skip = 0,
        count = 0,
        num = count_tokens_from_text(text, SPACE);

    if (num < 0)
        return NULL;

    char **out = (char **)malloc(sizeof(char *) * num);

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
                free_tokens(out, count + 1);
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
