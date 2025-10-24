#ifndef WRITER_DATA_BLOCK_H
#define WRITER_DATA_BLOCK_H

#include "Types/types.h"
#include "Metadata/metadata.h"

/**
 * Takes a datablock and creates a writable character array that can be written into file
 */
char *BuildWritableFromDataBlock(DataBlock *block, TableDefinition *def);

#endif
