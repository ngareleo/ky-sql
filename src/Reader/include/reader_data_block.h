#ifndef READER_DATA_BLOCK_H
#define READER_DATA_BLOCK_H

#include "Types/types.h"
#include "Metadata/metadata.h"

/**
 * Create a data block from raw data that has been read from file
 */
DataBlock *BuildDataBlockFromReadable(char *, TableDefinition *);
TableDefinition *MatchTableDefFromLiqsmt(Liqsmt *, SchemaDefinition *);
TableColDefinition *MatchColDefFromLiqsmt(char *, TableDefinition *);
#endif