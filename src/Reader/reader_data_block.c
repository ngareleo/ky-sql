#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/reader_data_block.h"
#include "include/reader_file.h"
#include "Metadata/metadata.h"
#include "Lang/language.h"
#include "Types/types.h"
#include "Utilities/utilities.h"
#include "Writer/writer.h"

int _Validate(Liqsmt *smt, ReaderMetadata *reader, FileMetadata *meta)
{
    if (!smt || !reader || !meta)
    {
        fprintf(stderr, "(validate-build-data-block-from-readable) Args are null \n");
        return -1;
    }

    if (!smt->Columns || smt->ColCount < 1)
    {
        fprintf(stderr, "(validate-build-data-block-from-readable) Statement is missing columns \n");
        return -1;
    }

    if (!reader->ReadBuffer || strlen(reader->ReadBuffer) < 1)
    {
        fprintf(stderr, "(validate-build-data-block-from-readable) No data read from file \n");
        return -1;
    }

    return 0;
}

/**
 * The data section is structured as contigous bytes of data separated by carriage returns
 * Since we know the schema, and the start of the table, we can separate the blocks precisely during reads to get specific columns/rows
 */
DataBlock *BuildDataBlockFromReadable(Liqsmt *smt, ReaderMetadata *reader, FileMetadata *meta)
{
    if (_Validate(smt, reader, meta) != 0)
    {
        fprintf(stderr, "(build-data-block-from-readable) Args are invalid \n");
        return NULL;
    }

    TableDefinition *tDef = MatchTableDefFromLiqsmt(smt, meta->Schema);
    if (!tDef)
    {
        fprintf(stderr, "(build-data-block-from-readable) Table not found in schema \n");
        return NULL;
    }

    int **colIds = CreateOrderedColIds(smt, tDef);
    if (!colIds)
    {
        fprintf(stderr, "(build-data-block-from-readable) Error ordering the data. The data might be invalid\n");
        return NULL;
    }

    // !! Walking down the path of least resistance
    // !! =========================================
    // !!
    // !! I want to get over this part quickly mostly because I'm unsure performant this architecture is.
    // !! For now, we make a BIG assumption that the data we write is accurately structured.
    // !! We enforce this rule strictly during writing to avoid faults.
    // !!
    // !! Even in the future I don't see this changing, until when I start to improve the internal structures to something like B-Trees
    // !! For now the focus is to understand the execution part and not storage
    // !! If I can limit coupling, this will be easy to change in future

    int rowCount = 0;
    if (smt->Limit == 0)
    {
        // ?? Here is an example of the assumption I mentioned
        // ?? I don't want to do many per character reads.
        // ?? Instead, I'm opting to estimate the number of rows read based on the query and the existing row count
        // ?? The knowing the row size and the column sizes, I can do precise jumps
        // ?? This will also workout well in future when we tackle search queries
        for (int idx = 0; idx < meta->Schema->TableCount; idx++)
        {
            if (meta->Storage->Items[idx]->TableId == tDef->Id)
            {
                rowCount = meta->Storage->Items[idx]->Count;
            }
        }
    }

    Allocator *alloc;
    DataBlock *data;
    StorageMetaItem *item;

    for (int idx = 0; idx < meta->Schema->TableCount; idx++)
    {
        if (meta->Storage->Items[idx]->TableId == meta->Schema->TableDefs[idx]->Id)
        {
            item = meta->Storage->Items[idx];
        }
    }

    if (!item)
    {
        fprintf(stderr, "(build-data-block-from-readable) Missing storage info\n");
        return NULL;
    }

    data = DefaultBlock();
    if (!data)
    {
        fprintf(stderr, "(build-data-block-from-readable) Failed to create data block\n");
        return NULL;
    }

    alloc = MallocInit();
    data->Values = Malloc(sizeof(char **) * (rowCount + 1), alloc);
    for (int ridx = 0; ridx < rowCount; ridx++)
    {
        data->Values[ridx] = Malloc(sizeof(char *) * smt->ColCount, alloc);
        for (int cidx = 0; cidx < smt->ColCount; cidx++)
        {
            for (int icIdx = 0; icIdx < tDef->ColumnCount; icIdx++)
            {
                if (colIds[cidx] == tDef->Columns[cidx]->Id)
                {
                    TableColDefinition *col = tDef->Columns[cidx];
                    int start = (ridx * item->RowSize) + item->ColInfo[cidx]->Padding;

                    size_t size = GetDataTypeSize(col->Type);
                    // ?? Later we'll add a prefix of 8 bytes to hold the exact string length
                    // ?? This will also unlock variable strings
                    // ?? Issue is we waste space since we reserve per data size
                    // ?? Won't address that anytime soon

                    data->Values[ridx][cidx] = Malloc(size, alloc);
                    data->Values[ridx][size] = NULL;
                }
            }
        }
    }

    data->Header = Malloc(sizeof(char *) * (smt->ColCount + 1), alloc);
    data->Header[smt->ColCount] = NULL;

    for (int cidx = 0; cidx < smt->ColCount; cidx++)
    {
        data->Header[cidx] = Malloc(strlen(smt->Columns[cidx]) + 1, alloc);
    }

    if (!VerifyAlloc(alloc))
    {
        for (int ridx = 0; ridx < rowCount; ridx++)
        {
            free(data->Values[ridx]);
            for (int cidx = 0; cidx < smt->ColCount; cidx++)
            {
                for (int icIdx = 0; icIdx < tDef->ColumnCount; icIdx++)
                {
                    if (colIds[cidx] == tDef->Columns[cidx]->Id)
                    {
                        free(data->Values[ridx][cidx]);
                    }
                }
            }
        }
        free(data->Values);
        for (int cidx = 0; cidx < smt->ColCount; cidx++)
        {
            free(data->Header[cidx]);
        }
        free(data->Header);
        FreeAlloc(alloc);
        FreeDataBlock(data);
    }

    for (int cidx = 0; cidx < smt->ColCount; cidx++)
    {
        strcpy(data->Header[cidx], smt->Columns[cidx]);
    }

    ValidateDataBlock(data);

    if (rowCount == 0)
    {
        return data;
    }

    for (int ridx = 0; ridx < rowCount; ridx++)
    {
        for (int cidx = 0; cidx < smt->ColCount; cidx++)
        {
            for (int icIdx = 0; icIdx < tDef->ColumnCount; icIdx++)
            {
                if (colIds[cidx] == tDef->Columns[cidx]->Id)
                {
                    TableColDefinition *col = tDef->Columns[cidx];
                    int start = (ridx * item->RowSize) + item->ColInfo[cidx]->Padding;
                    size_t size = GetDataTypeSize(col->Type);
                    strncpy(data->Values[ridx][cidx],
                            reader->ReadBuffer[start],
                            sizeof(data->Values[ridx][cidx]));
                }
            }
        }
    }

    ValidateDataBlock(data);
    FreeAlloc(alloc);
    return data;
}

TableDefinition *MatchTableDefFromLiqsmt(Liqsmt *smt, SchemaDefinition *schema)
{
    if (!smt || !schema)
    {
        fprintf(stderr, "(match-table-def-from-liqsmt) args invalid \n");
        return -1;
    }

    TableDefinition *def = NULL;
    for (int tIdx = 0; tIdx < schema->TableCount; tIdx++)
    {
        if (strcmp(schema->TableDefs[tIdx]->Name, smt->TableName) == 0)
        {
            def = schema->TableDefs[tIdx];
        }
    }

    return def;
}

TableColDefinition *MatchColDefFromLiqsmt(char *colName, TableDefinition *def)
{
    if (!colName || !def)
    {
        fprintf(stderr, "(match-col-def-from-liqsmt) args invalid \n");
        return -1;
    }

    TableColDefinition *def = NULL;
    for (int cIdx = 0; cIdx < def->ColumnCount; cIdx++)
    {
        if (strcmp(def->Columns[cIdx], colName) == 0)
        {
            def = def->Columns;
        }
    }

    return def;
}

int **MatchColsFromLiqsmt(Liqsmt *smt, TableDefinition *def)
{
    if (!smt || !def)
    {
        fprintf(stderr, "(create-ordered-col-ids) Args are invalid\n");
        return NULL;
    }

    if (smt->ColCount < 1)
    {
        fprintf(stderr, "(create-ordered-col-ids) Columns are empty\n");
        return NULL;
    }

    int **cols;
    cols = malloc(sizeof(int *) * (smt->ColCount + 1));
    if (!cols)
    {
        fprintf(stderr, "(create-ordered-col-ids) Malloc failed\n");
        return NULL;
    }

    cols[smt->ColCount] = NULL;

    for (int cIdx = 0; cIdx < smt->ColCount; cIdx++)
    {
        TableColDefinition *col = MatchColDefFromLiqsmt(smt->Columns[cIdx], def);
        if (!col)
        {
            fprintf(stderr, "(create-ordered-col-ids) Col not found in schema \n");
            return NULL;
        }
        *cols[cIdx] = col->Id;
    }

    return cols;
}

int **CreateOrderedColIds(Liqsmt *smt, TableDefinition *def)
{
    if (!smt || !def)
    {
        fprintf(stderr, "(create-ordered-col-ids) Args are invalid\n");
        return NULL;
    }

    int count = smt->ColCount, colC = 0;
    int **orderedCols = malloc(sizeof(int *) * (count + 1));
    int **colIds = MatchColsFromLiqsmt(smt, def);

    orderedCols[count] = NULL;

    if (smt->Order)
    {
        for (int oIdx = 0; oIdx < smt->OrderCount; oIdx)
        {
            for (int cIdx = 0; cIdx < def->ColumnCount; cIdx++)
            {
                if (strcmp(smt->Order[oIdx], def->Columns[cIdx]->Name) == 0)
                {
                    *orderedCols[colC++] = def->Columns[cIdx]->Id;
                }
            }
        }
    }

    for (int cIdx = 0; cIdx < smt->ColCount; cIdx++)
    {
        bool seen = false;
        for (int ecIdx = 0; ecIdx < colC; ecIdx++)
        {
            if (orderedCols[ecIdx] == colIds[cIdx])
            {
                seen = true;
            }
        }

        if (!seen)
        {
            *orderedCols[colC++] = *colIds[cIdx];
        }
    }

    return orderedCols;
}