#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/translation_context.h"
#include "include/translation_insert.h"
#include "Metadata/metadata.h"
#include "Lang/language.h"
#include "Reader/reader.h"
#include "Types/types.h"
#include "Utilities/utilities.h"
#include "Writer/writer.h"

int LinsmtToWriteRequest(Linsmt *stmt, WriteRequest **req)
{
    WriteRequest *writeRQ;
    TranslationContext *ctx = GetTranslationContext();

    if (!ctx)
    {
        fprintf(stderr, "(linsmt-to-write-request) translation context unavailable \n");
        return -1;
    }

    if (ValidateLinsmt(stmt) != 0)
    {
        fprintf(stderr, "(linsmt-to-write-request) statement failed validation \n");
        return -1;
    }

    if (SchemaValidateLinsmt(stmt, ctx) != 0)
    {
        fprintf(stderr, "(linsmt-to-write-request) statement failed schema validation \n");
        return -1;
    }

    writeRQ = CreateWriteRequest(stmt, ctx);
    if (!writeRQ)
    {
        fprintf(stderr, "(linsmt-to-write-request) failed to create write request \n");
        return -1;
    }

    *req = writeRQ;
    return 0;
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

WriteRequest *CreateWriteRequest(Linsmt *stmt, TranslationContext *ctx)
{
    if (!stmt || !ctx)
    {
        fprintf(stderr, "(create-write-request) args are invalid \n");
        return -1;
    }

    TableDefinition *tableDef;
    WriteRequest *req;

    char *writable = TranslateLinsmt(stmt, ctx);
    if (!writable)
    {
        fprintf(stderr, "(linsmt-to-write-request) couldn't generate a writable \n");
        return -1;
    }

    req = malloc(sizeof(WriteRequest));
    if (!req)
    {
        fprintf(stderr, "(create-write-request) malloc failed \n");
        return -1;
    }

    req->Payload = malloc(strlen(writable) + 1);
    if (!req->Payload)
    {
        fprintf(stderr, "(create-write-request) malloc failed \n");
        return -1;
    }

    strcpy(req->Payload, writable);

    tableDef = MatchTableDefFromLinsmt(stmt, ctx->FileMetadata->Schema);
    for (int c = 0; c < ctx->FileMetadata->Offset->TableCount; c++)
    {
        if (tableDef->Id == ctx->FileMetadata->Offset->Offsets[c]->Id)
        {
            TableOffset *off = ctx->FileMetadata->Offset->Offsets[c]->Offset;
            req->StartPosition = off->Offset;
        }
    }

    return req;
}

char *TranslateLinsmt(Linsmt *stmt, TranslationContext *ctx)
{
    if (!stmt || !ctx)
    {
        fprintf(stderr, "(translate-linsmt) args are invalid \n");
        return -1;
    }

    FILE *stream;
    char *buffer;
    size_t size;

    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, ctx->FileMetadata->Schema);
    if (!tableDef)
    {
        return -1;
    }

    stream = open_memstream(&buffer, &size);
    if (!stream)
    {
        fprintf(stderr, "(translate-linsmt) open_memstream failed.\n");
        return NULL;
    }

    for (int rowIdx = 0; rowIdx < stmt->Data->Size->Count; rowIdx++)
    {
        for (int colIdx = 0; colIdx < stmt->Data->Size->Width; colIdx)
        {
            for (int scIdx = 0; scIdx < tableDef->Columns; scIdx++)
            {
                char *val = stmt->Data->Values[rowIdx][colIdx];
                char *tableName = stmt->Data->Header[colIdx];

                TableColDefinition *colDef = MatchTableColFromLinsmt(tableName, tableDef);
                if (colDef)
                {
                    switch (colDef->Type)
                    {
                    case ID:
                        fwrite(val, strlen(val + 1), 1, stream);
                        break;
                    case BOOL:
                        fwrite(val, strlen(val + 1), 1, stream);
                        break;
                    case INTEGER:
                        fwrite(val, strlen(val + 1), 1, stream);
                        break;
                    case FLOAT:
                        fwrite(val, strlen(val + 1), 1, stream);
                        break;
                    case STRING:
                        fwrite(val, strlen(val + 1), 1, stream);
                        break;
                    case DATE:
                        fwrite(val, strlen(val + 1), 1, stream);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    fflush(stream);
    fclose(stream);
    return buffer;
}

int SchemaValidateLinsmt(Linsmt *stmt, TranslationContext *ctx)
{
    if (!ctx || !stmt)
    {
        fprintf(stderr, "(schema-validate-linsmt) context is invalid \n");
        return -1;
    }

    bool colsFound = false;
    SchemaDefinition *schema = ctx->FileMetadata->Schema;

    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, schema);
    if (!tableDef)
    {
        return -1;
    }

    for (int stmtC = 0; stmtC < stmt->Data->Size->HeaderCount; stmtC++)
    {
        TableColDefinition *colDef = MatchTableColFromLinsmt(stmt->Data->Header[stmtC], tableDef);
        if (!colDef)
        {
            return -1;
        }
    }

    return 0;
}

int ValidateLinsmt(Linsmt *smt)
{
    if (!smt)
    {
        fprintf(stderr, "(validate-linsmt) statment provided is null \n");
        return -1;
    }

    if (!smt->Data || !smt->Data->IsValid)
    {
        fprintf(stderr, "(validate-linsmt) statement is invalid \n");
        return -1;
    }

    if (smt->Data->IsEmpty)
    {
        fprintf(stderr, "(validate-linsmt) statement is empty  \n");
        return -1;
    }

    if (!smt->TableName)
    {
        fprintf(stderr, "(validate-linsmt) tablename is invalid \n");
        return -1;
    }

    return 0;
}