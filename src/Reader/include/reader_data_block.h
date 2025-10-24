#ifndef READER_DATA_BLOCK_H
#define READER_DATA_BLOCK_H

#include "Metadata/metadata.h"
#include "Types/types.h"

DataBlock *BuildDataBlockFromReadable(Liqsmt *, ReaderMetadata *, FileMetadata *);
TableDefinition *MatchTableDefFromLiqsmt(Liqsmt *, SchemaDefinition *);
TableColDefinition *MatchColDefFromLiqsmt(char *, TableDefinition *);

#endif