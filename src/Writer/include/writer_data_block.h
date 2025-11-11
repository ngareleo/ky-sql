#ifndef WRITER_DATA_BLOCK_H
#define WRITER_DATA_BLOCK_H

#include "Types/types.h"
#include "Metadata/metadata.h"

char *BuildWritableFromDataBlock(Linsmt *, FileMetadata *);
TableColDefinition *MatchTableColFromLinsmt(char *, TableDefinition *);
TableDefinition *MatchTableDefFromLinsmt(Linsmt *, SchemaDefinition *);

#endif
