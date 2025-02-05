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
    size_t len, i, start, end;
    char *out;

    if (text == NULL)
        return NULL;

    len = strlen(text);
    start = 0, end = len - 1;

    if (len == 0)
        return strdup("");

    while (*(text + start) == SPACE)
        start++;

    while (*(text + end) == SPACE)
        end--;

    // printf("\n{ Word: '%s', Len: %d, Start: %d, End: %d }\n", text, len, start, end);

    if (start == len)
        return strdup("");

    // copy over from start to end
    out = malloc(sizeof(char) * end - start + 1);
    if (out == NULL)
        return NULL;

    for (i = 0; start <= end; i++, start++)
        out[i] = text[start];

    out[i] = '\0';

    // printf("\n{ Output: '%s' }\n", out);
    return out;
}

int count_tokens_from_text(char *text, char match)
{
    size_t n;
    char *s_text = strip(text), *curr;
    int num_s, // Number of tokens so far
        skip;  // should skip?

    if (text == NULL)
        return -1;

    if (s_text == NULL)
        return -1;

    n = strlen(s_text);
    if (n <= 0)
        return -1;

    skip = 0;
    num_s = 1;
    for (curr = s_text; *curr != '\0'; curr++)
    {
        // printf("\n { Current: %c }", *curr);
        if (*curr == match)
        {
            if (skip)
                continue;

            num_s++;
            skip = 1; // indicate that we have hit some space
        }
        else
            skip = 0; // indicate that we have hit some non-space and can start counting tokens
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
