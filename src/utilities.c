// Utility functions that are decoupled from the application
#include <unistd.h>
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

char *strip(char *text)
{
    size_t len = strlen(text);
    int start = 0, end = len - 1;

    if (len == 0)
        return NULL;

    while (*(text + start) == SPACE)
        start++;

    while (*(text + end) == SPACE)
        end--;

    // printf("\n{ Word: '%s', Len: %d, Start: %d, End: %d }\n", text, len, start, end);

    if (start == len)
        return "";

    // copy over from start to end
    char *out = malloc(sizeof(char) * end - start);

    for (int i = 0; start <= end; i++, start++)
        out[i] = text[start];

    // printf("\n{ Output: '%s' }\n", out);
    return out;
}

int count_tokens_from_text(char *text, char match)
{
    if (text == NULL)
        return -1;
    char *s_text = strip(text);
    if (s_text == NULL)
        return -1;

    size_t n = strlen(s_text);
    if (n <= 0)
        return -1;

    int num_s = 1, // Number of tokens so far
        skip = 0;

    for (char *curr = s_text; *curr != '\0'; curr++)
    {
        // printf("\n { Current: %c }", *curr);
        if (*curr == match)
        {
            if (skip)
            {
                continue;
            }
            num_s++;
            // indicate that we have hit some space
            skip = 1;
        }
        else
        {
            // indicate that we have hit some non-space and can start counting tokens
            skip = 0;
        }
    }

    // printf("\n{ Word: %s, Len: %zu, Num: %d }\n", s_text, n, num_s);
    return num_s;
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
