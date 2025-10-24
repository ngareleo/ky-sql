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

int LinsmtToWriteRequest(Linsmt *smt, WriteRequest **req)
{
    WriteRequest *wReq;
    TranslationCtx *ctx = GetTranslationContext();

    if (!ctx)
    {
        fprintf(stderr, "(linsmt-to-write-request) translation context unavailable \n");
        return -1;
    }

    if (ValidateLinsmt(smt) != 0)
    {
        fprintf(stderr, "(linsmt-to-write-request) statement failed validation \n");
        return -1;
    }

    if (SchemaValidateLinsmt(smt, ctx) != 0)
    {
        fprintf(stderr, "(linsmt-to-write-request) statement failed schema validation \n");
        return -1;
    }

    wReq = CreateWriteRequest(smt, ctx);
    if (!wReq)
    {
        fprintf(stderr, "(linsmt-to-write-request) failed to create write request \n");
        return -1;
    }

    *req = wReq;

    if (!UpdateStorageFromLinsmt(smt, ctx) != 0)
    {
        FreeWriteRequest(wReq);
        fprintf(stderr, "(linsmt-to-write-request) failed to update storage \n");
        return -1;
    }

    return 0;
}

int UpdateStorageFromLinsmt(Linsmt *smt, TranslationCtx *ctx)
{
    if (!smt || !ctx)
    {
        fprintf(stderr, "(update-storage-from-linsmt) args are invalid \n");
        return -1;
    }

    TableDefinition *def = MatchTableDefFromLinsmt(smt, ctx->FileMd->Schema);
    if (!def)
    {
        fprintf(stderr, "(update-storage-from-linsmt) table doesn't exist \n");
        return -1;
    }

    for (int idx = 0; idx < ctx->FileMd->Schema->TableCount; idx++)
    {
        if (ctx->FileMd->Storage->Items[idx]->TableId == def->Id)
        {
            ctx->FileMd->Storage->Items[idx]->Count += smt->Data->Size->Count;
            return 0;
        }
    }

    return -1;
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

WriteRequest *CreateWriteRequest(Linsmt *stmt, TranslationCtx *ctx)
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

    tableDef = MatchTableDefFromLinsmt(stmt, ctx->FileMd->Schema);
    for (int c = 0; c < ctx->FileMd->Offset->TableCount; c++)
    {
        if (tableDef->Id == ctx->FileMd->Offset->Offsets[c]->Id)
        {
            TableOffset *off = ctx->FileMd->Offset->Offsets[c]->Offset;
            req->StartPosition = off->Offset;
        }
    }

    return req;
}

char *TranslateLinsmt(Linsmt *stmt, TranslationCtx *ctx)
{
    if (!stmt || !ctx)
    {
        fprintf(stderr, "(translate-linsmt) args are invalid \n");
        return -1;
    }

    FILE *stream;
    char *buffer;
    size_t size;

    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, ctx->FileMd->Schema);
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

    for (int rowIdx = 0; rowIdx < stmt->Data->Size->Count; rowIdx)
    {
        for (int scIdx = 0; scIdx < tableDef->ColumnCount; scIdx++)
        {
            for (int colIdx = 0; colIdx < stmt->Data->Size->Width; colIdx++)
            {
                TableColDefinition *colDef = MatchTableColFromLinsmt(stmt->Data->Header[colIdx], tableDef);
                char *val = colDef ? stmt->Data->Values[rowIdx][colIdx] : "";
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

int SchemaValidateLinsmt(Linsmt *stmt, TranslationCtx *ctx)
{
    if (!ctx || !stmt)
    {
        fprintf(stderr, "(schema-validate-linsmt) context is invalid \n");
        return -1;
    }

    SchemaDefinition *schema = ctx->FileMd->Schema;
    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, schema);
    if (!tableDef)
    {
        return -1;
    }

    for (int colIdx = 0; colIdx < stmt->Data->Size->HeaderCount; colIdx++)
    {
        TableColDefinition *colDef = MatchTableColFromLinsmt(stmt->Data->Header[colIdx], tableDef);
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