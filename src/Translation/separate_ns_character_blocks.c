#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Utilities/utilities.h"

const char *delim = "\r";

/**
 * Validates that the text provided from language layer is well structured
 * !! We will do this at the language layer in future, so no need to check here
 */
int ValdidateBlockStructure(char **raw)
{
    if (!raw)
    {
        fprintf(stderr, "(validate-block-structure) Invalid text\n");
        return -1;
    }

    int c, count, sizing;
    count = Count((void **)raw);
    if (count < 1)
    {
        fprintf(stderr, "(validate-block-structure) Invalid text\n");
        return -1;
    }

    for (c = 0; c < count; c++)
    {
        int tokenCount = 0;
        char *target;
        if (raw[c] && (target = malloc(strlen(raw[c]) + 1)))
        {
            strcpy(target, raw[c]);
            while (strtok(target, delim))
            {
                tokenCount++;
            }
        }
        else
        {
            fprintf(stderr, "(validate-block-structure) Error during validation\n");
            return -1;
        }

        if (!sizing)
        {
            tokenCount = sizing;
        }
        else if (sizing != tokenCount)
        {
            fprintf(stdout, "(validate-block-structure) Block structure invalid\n");
            return -1;
        }
    }

    return 0;
}

typedef struct
{
    int x;
    int y;
} BlockSize;

/**
 * It estimates the block size of the
 */
BlockSize *EstimateBlockSize(char **raw)
{
    if (!raw || Count((void **)raw) < 0 || ValdidateBlockStructure(raw) != 0)
    {
        fprintf(stdout, "(estimate-block-size) Invalid text\n");
        return NULL;
    }

    BlockSize *size;
    size = malloc(sizeof(BlockSize));
    if (!size)
    {
        return NULL;
    }

    char *target;
    char *token;
    int x = 0;

    target = malloc(strlen(raw[0]) + 1);
    strcpy(target, raw[0]);
    token = strtok(target, delim);

    while (token)
    {
        x++;
        token = strtok(NULL, delim);
    }

    size->x = x;
    size->y = Count((void **)raw);
    return size;
}

/**
 * Takes raw blocked text separated by carriage returns `/r`
 * Returns a structured multidimensional array
 * Preserves order
 */
char ***SeparateColumnBlocks(char **raw)
{
    if (!raw || strlen(raw) < 1 || ValdidateBlockStructure(raw) != 0)
    {
        fprintf(stderr, "(separate-column-blocks) Invalid text\n");
        return NULL;
    }

    int c, count;
    count = Count((void **)raw);
    if (count < 1)
    {
        fprintf(stderr, "(separate-column-blocks) Invalid text\n");
        return -1;
    }

    BlockSize *size = EstimateBlockSize(raw);
    if (!size)
    {
        return NULL;
    }

    // pre-allocate
    char ***finalTokens;
    finalTokens = malloc(sizeof(char **) * size->y + 1);
    if (!finalTokens)
    {
        fprintf(stderr, "(separate-column-blocks) malloc failed\n");
        return -1;
    }

    for (c = 0; c < count; c++)
    {
        int row_c = 0;
        char *target, **row, *token;
        row = malloc(sizeof(char **) * size->x + 1);
        if (row && raw[c] && (target = malloc(strlen(raw[c]) + 1)))
        {
            strcpy(target, raw[c]);
            while ((token = strtok(target, delim)))
            {
                row[row_c] = malloc(strlen(token) + 1);
                if (!row[row_c])
                {
                    // Cleanup row allocations with current turn
                    for (int c_j = 0; c_j < row_c; c_j++)
                    {
                        free(row[c_j]);
                    }
                    free(row);

                    // cleanup previous row allocations
                    for (int c_i = 0; c_i < c; c_i++)
                    {
                        for (int c_i_2 = 0; c_i_2 < size->x; c_i_2++)
                        {
                            free(finalTokens[c_i][c_i_2]);
                        }
                        free(finalTokens[c_i]);
                    }

                    free(finalTokens);
                    free(size);
                    fprintf(stderr, "(separate-column-blocks) malloc failed\n");
                    return NULL;
                }
                row_c++;
            }
            row[row_c] = NULL;
            finalTokens[c] = row;
        }
        else
        {
            // cleanup previous row allocations
            for (int c_i = 0; c_i < c; c_i++)
            {
                for (int c_i_2 = 0; c_i_2 < size->x; c_i_2++)
                {
                    free(finalTokens[c_i][c_i_2]);
                }
                free(finalTokens[c_i]);
            }
            free(finalTokens);
            free(size);
            fprintf(stderr, "(validate-block-structure) Error during validation\n");
            return NULL;
        }
    }

    finalTokens[size->y] = NULL;
    free(size);
    return finalTokens;
}