// Utility functions that are decoupled from the application

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Split input by whitespace. Takes in `out` which it reserves memory for.
 * It's upto to caller to free memory at the end.
 */
int tokenize(char *in, char **out)
{
    size_t n;
    if ((n = strlen(in)) <= 0)
        return -1;

    int ns = 0;
    for (int i = 0; i < n; i++)
    {
        if (*(in + i) == ' ')
            ns++;
    }

    out = (char **)malloc(sizeof(char *) * ns);

    char *p_out = *out;
    int marker = 0, count = 0;

    while (count < n)
    {
        if (*(in + count) == ' ' || count == n + 1)
        {
            if (count != marker)
            {
                p_out = (char *)malloc(sizeof(char) * (count - marker));
                memcpy(p_out, in + marker, count - marker);
                p_out++;
                marker = count + 1;
            }
        }
        count++;
    }

    return 0;
}
