#include <stdio.h>
#include <stdlib.h>
#include "include/writer_size.h"
#include "Metadata/metadata.h"

size_t GetDataTypeSize(enum SchemaType type)
{
    switch (type)
    {
    case ID:
        return sizeof(long);
    case BOOL:
        return sizeof(int);
    case INTEGER:
        return sizeof(int);
    case FLOAT:
        return sizeof(float);
    case STRING:
        return MAX_STRING_SIZE;
    case DATE:
        return sizeof(int);
    default:
        return 0;
    }
}