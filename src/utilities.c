// Utility functions that are decoupled from the application
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPACE ' '
#define SEMICOLON ';'

typedef char *Token;
typedef char **Tokens;

void pr_tokens(Tokens tokens)
{
    int count;

    if (tokens == NULL)
        return;

    printf("\n[ ");
    for (count = 0; tokens[count] != NULL; count++)
        printf("'%s', ", tokens[count]);
    printf("]\n");
}

int count_tokens_flat_list(Tokens tokens)
{
    int count;

    if (tokens == NULL)
        return -1;

    for (count = 0; tokens[count] != NULL; count++)
        ;

    return count;
}

void free_tokens(Tokens tokens)
{
    int count;

    if (tokens == NULL)
        return;

    for (count = 0; tokens[count] != NULL; count++)
        free(tokens[count]);

    free(tokens);
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

    // printf("\n{ Word: '%s', Len: %zu, Start: %zu, End: %zu }\n", text, len, start, end);

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
    free(s_text);
    return num_s;
}

char **tokenize(char *text)
{

    int skip,
        count,
        len, // len of text
        n_tokens = count_tokens_from_text(text, SPACE);

    char **output,
        *marker, // marks word boundaries to copy over
        *chars,  // used by loop to step through the string
        *s_text; // stripped down text

    if (text == NULL)
        return NULL;

    if (n_tokens < 0)
        return NULL;

    len = strlen(text);
    if (*(text + len - 1) != ';') // should terminate string with semicolon
        return NULL;

    output = (char **)malloc(sizeof(char *) * n_tokens + 1);

    if (output == NULL)
        return NULL;

    skip = count = 0;
    s_text = strip(text);
    marker = chars = s_text;

    printf("\n { Word: '%s', Stripped: '%s', NumOfTokens: %d } \n", text, s_text, n_tokens);

    for (; *chars != '\0'; chars++)
    {
        if (*chars == SPACE || *chars == SEMICOLON)
        {
            if (skip)
            {
                marker++; // move marker forward as well
                continue;
            }

            int width = chars - marker; // the word width

            output[count] = (char *)malloc(sizeof(char) * width + 1);
            if (output[count] == NULL)
            {
                free_tokens(output);
                return NULL;
            }
            strncpy(output[count], marker, width);
            output[count][width] = '\0';
            printf("\n { New Word: '%s',  Width: %d } \n", output[count], width);
            count++;
            marker = chars + 1;
            skip = 1; // indicate that we have hit some space
        }
        else
        {
            skip = 0;
        }
    }

    free(s_text);
    output[count] = NULL;
    return output;
}
