#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Split input by whitespace
 */
int tokenize(char *in, char *out[])
{
    size_t n;
    if ((n = strlen(in)) <= 0)
    {
        return -1;
    }

    int marker = 0, count = 0;
    while (count < n)
    {
        if (*(in + count) == ' ')
        {
            memmove(*out, in + count, count - marker);
            out++;
            marker = count;
            printf("Value --> %s\n", *out);
        }
        count++;
    }

    return 0;
}
