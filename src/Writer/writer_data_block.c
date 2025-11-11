#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lang/language.h"
#include "Writer/writer.h"
#include "Metadata/metadata.h"
#include "Types/types.h"

int _Validate(Linsmt *smt, FileMetadata *meta)
{
    if (!smt || !meta)
    {
        fprintf(stderr, "(validate-build-writable-from-data-block) Args are null \n");
        return -1;
    }

    if (!smt->Data || smt->Data->Size->Count < 1 || smt->Data->IsEmpty)
    {
        fprintf(stderr, "(validate-build-writable-from-data-block) Data is empty \n");

        return -1;
    }

    return 0;
}

char *BuildWritableFromDataBlock(Linsmt *smt, FileMetadata *meta)
{
    if (!_Validate(smt, meta))
    {
        fprintf(stderr, "(validate-build-writable-from-data-block) Args are invalid\n");
        return -1;
    }

    TableDefinition *tDef = MatchTableDefFromLinsmt(smt, meta->Schema);
    if (!tDef)
    {
        fprintf(stderr, "(validate-build-writable-from-data-block) Table not found in schema \n");
        return -1;
    }

    FILE *stream;
    char *buffer;
    size_t size;

    stream = open_memstream(&buffer, &size);
    if (!stream)
    {
        fprintf(stderr, "(translate-linsmt) open_memstream failed.\n");
        return NULL;
    }

    for (int ridx = 0; ridx < smt->Data->Size->Count; ridx)
    {
        for (int ccidx = 0; ccidx < tDef->ColumnCount; ccidx++)
        {
            for (int colidx = 0; colidx < smt->Data->Size->Width; colidx++)
            {
                TableColDefinition *colDef = MatchTableColFromLinsmt(smt->Data->Header[colidx], tDef);
                char *val = colDef ? smt->Data->Values[ridx][colidx] : "";
                if (WriteColumnEntry(val, stream, colDef->Type) < 1)
                {
                    fprintf(stderr, "(translate-linsmt) write failed.\n");
                    fclose(stream);
                    return NULL;
                }
            }
        }
    }

    fflush(stream);
    fclose(stream);
    return buffer;
}

TableColDefinition *MatchTableColFromLinsmt(char *name, TableDefinition *def)
{
    if (!name || !def)
    {
        fprintf(stderr, "(match-table-col-from-linsmt) args are invalid \n");
        return -1;
    }

    TableColDefinition *colDef;
    for (int col = 0; col < def->ColumnCount; col++)
    {
        if (strcmp(name, def->Columns[col]->Name) == 0)
        {
            colDef = def->Columns[col];
        }
    }

    return colDef;
}

TableDefinition *MatchTableDefFromLinsmt(Linsmt *stmt, SchemaDefinition *def)
{
    if (!stmt || !def)
    {
        fprintf(stderr, "(match-table-col-from-linsmt) args are invalid \n");
        return -1;
    }

    TableDefinition *tableDef;
    for (int tIdx = 0; tIdx < def->TableCount; tIdx++)
    {
        if (strcmp(def->TableDefs[tIdx]->Name, stmt->TableName) == 0)
        {
            tableDef = def->TableDefs[tIdx];
        }
    }

    return tableDef;
}