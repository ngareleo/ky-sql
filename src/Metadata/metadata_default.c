#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "metadata_schema.h"

void *CreateDefaultValue(enum SchemaType schemaType, const char *defaultValue)
{
    void *output;
    if (!defaultValue)
    {
        fprintf(stderr, "(create-default-value-err) `defaultValue` is null. \n");
        return -1;
    }

    switch (schemaType)
    {
    case DATE:
        output = malloc(sizeof(time_t));
        if (output)
        {
            *(time_t *)output = atol(defaultValue);
            return output;
        }
        break;

    case INTEGER:
        output = malloc(sizeof(int));
        if (output)
        {
            *(int *)output = atoi(defaultValue);
            return output;
        }
        break;

    case STRING:
        output = malloc(strlen(defaultValue) + 1);
        if (output)
        {
            strcpy(output, defaultValue);
            return output;
        }
        break;

    case FLOAT:
        output = malloc(sizeof(double));
        if (output)
        {
            *(double *)output = atof(defaultValue);
            return output;
        }
        break;

    case BOOL:
        output = malloc(sizeof(bool));
        if (output)
        {
            *(bool *)output = strcmp(defaultValue, &"0") ? true : false;
            return output;
        }
        break;

    default:
        break;
    }

    fprintf(stderr, "(create-default-value-err) malloc failed. \n");
    return NULL;
}
