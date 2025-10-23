#include <stdio.h>
#include <stdlib.h>
#include "include/writer_size.h"

int WriteColumnEntry(char *ptr, FILE *file, enum SchemaType type)
{
    switch (type)
    {
    case ID:
        long rVal = atol(ptr);
        return fwrite(&rVal, GetDataTypeSize(type), 1, file);
    case BOOL:
        int rVal = atoi(ptr);
        return fwrite(&rVal, GetDataTypeSize(type), 1, file);
    case INTEGER:
        int rVal = atoi(ptr);
        return fwrite(&rVal, GetDataTypeSize(type), 1, file);
    case FLOAT:
        float rVal = atof(ptr);
        return fwrite(&rVal, GetDataTypeSize(type), 1, file);
    case STRING:
        fwrite(&rVal, GetDataTypeSize(type), 1, file);
        break;
    case DATE:
        int rVal = atoi(ptr);
        return fwrite(&rVal, GetDataTypeSize(type), 1, file);
    default:
        break;
    }
}
