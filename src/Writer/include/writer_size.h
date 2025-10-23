#ifndef WRITER_SIZE_H
#define WRITER_SIZE_H

#include <stdio.h>
#include <stdlib.h>
#include "Metadata/metadata.h"

#define MAX_STRING_SIZE 1000000

size_t GetDataTypeSize(enum SchemaType);

#endif