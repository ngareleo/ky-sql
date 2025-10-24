#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/reader_data_block.h"
#include "include/reader_file.h"
#include "Metadata/metadata.h"
#include "Lang/language.h"
#include "Types/types.h"

int _Validate(Liqsmt *smt, TableDefinition *tDef, ReaderMetadata *reader, FileMetadata *meta)
{
    if (!smt || !reader || !meta || !tDef)
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

/** The data section is structured as contigous bytes of data separated by carriage returns
 * Since we know the schema, and the start of the table, we can separate the blocks precisely during reads to get specific columns/rows */
DataBlock *BuildDataBlockFromReadable(Liqsmt *smt, TableDefinition *tDef, ReaderMetadata *reader, FileMetadata *meta)
{
    if (_Validate(smt, tDef, reader, meta) != 0)
    {
        fprintf(stderr, "(build-data-block-from-readable) Args are invalid \n");
        return NULL;
    }

    int *colIds = CreateOrderedColIds(smt, tDef);
    if (!colIds)
    {
        fprintf(stderr, "(build-data-block-from-readable) Error ordering the data. The data might be invalid\n");
        return NULL;
    }

    // !! Path of least resistance
    // !! I want to get over this part quickly mostly because I'm unsure how to proceed
    // !! For now,
    // !! We make a hard assumption that the data we write is well structured.
    // !! We will enforce this rule during writing
    // !! Given that we can compute exact read locations from the schema
    // !! We will use byte location to control reading and hence construct data blocks
    // !! Even in the future I don't see this changing
    // !! Until I improve the structure in future to something like B-Trees
    // !! For now the focus is to understand the execution part and not storage.

    // ?? Here is an example of the assumption I mentioned
    // ?? I don't want to do single character reads and instead do a whole data strucute read
    // ?? I will do a guess of the number of rows read based on limit, max-read size or total storage count

    int rowCount = 0;
    if (smt->Limit == 0)
    {
        // We don't have a read size limit
        // So we check the row count
        for (int idx = 0; idx < meta->Schema->TableCount; idx++)
        {
            if (meta->Storage->Items[idx]->TableId == tDef->Id)
            {
                rowCount = meta->Storage->Items[idx]->Count;
            }
        }
    }

    if (rowCount == 0)
    {
        DataBlock *emp = DefaultBlock();
        emp->Header = malloc(sizeof(char *) * (smt->ColCount + 1));
        if (!emp || !emp->Header)
        {
            fprintf(stderr, "(build-data-block-from-readable) Error creating data block\n");
            return NULL;
        }

        emp->Header[smt->ColCount] = NULL;
        for (int cidx = 0; cidx < smt->ColCount; cidx++)
        {
            emp->Header[cidx] = malloc(strlen(smt->Columns[cidx]) + 1);
            if (emp->Header[cidx])
            {
                free(emp->Header);
                free(emp);
                fprintf(stderr, "(build-data-block-from-readable) Malloc failed\n");
                return NULL;
            }

            strcpy(emp->Header[cidx], smt->Columns[cidx]);
        }

        ValidateDataBlock(emp);
        return emp;
    }

    for (int ridx = 0; ridx < rowCount; ridx++)
    {
    }
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