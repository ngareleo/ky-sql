#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "translation_context.h"
#include "Lang/language.h"
#include "Metadata/metadata.h"
#include "Reader/reader.h"
#include "Writer/writer.h"

int LiqsmtToReadRequest(Liqsmt *smt, ReadRequest **req)
{
    ReadRequest *readReq;
    TranslationCtx *ctx = GetTranslationContext();

    if (!ctx)
    {
        fprintf(stderr, "(liqsmt-to-read-request) Translation context unavailable \n");
        return -1;
    }

    if (ValidateLiqsmt(smt) != 0)
    {
        fprintf(stderr, "(liqsmt-to-read-request) Statement validation failed \n");
        return -1;
    }

    if (SchemaValidateLiqsmt(smt, ctx) != 0)
    {
        fprintf(stderr, "(liqsmt-to-read-request) Schema validation failed \n");
        return -1;
    }

    readReq = CreateReadRequest(smt, ctx);
    if (!readReq)
    {
        fprintf(stderr, "(liqsmt-to-read-request) Failed to create read request \n");
        return -1;
    }

    *req = readReq;
    return 0;
}

ReadRequest *CreateReadRequest(Liqsmt *smt, TranslationCtx *ctx)
{
    if (!smt || !ctx)
    {
        fprintf(stderr, "(create-read-request) args invalid \n");
        return NULL;
    }

    ReadRequest *readReq;
    TableDefinition *tableDef = MatchTableDefFromLiqsmt(smt, ctx->FileMd->Schema);
    if (!tableDef)
    {
        fprintf(stderr, "(liqsmt-to-read-request) Table not found \n");
        return NULL;
    }

    for (int tIdx = 0; tIdx < ctx->FileMd->Offset->TableCount; tIdx++)
    {
        if (ctx->FileMd->Offset->Offsets[tIdx]->Id == tableDef->Id &&
            ctx->FileMd->Storage->Items[tIdx]->TableId == tableDef->Id)
        {
            if (ctx->FileMd->Storage->Items[tIdx]->Count == 0)
            {
                fprintf(stderr, "(liqsmt-to-read-request) File is empty \n");
                return NULL;
            }

            readReq = malloc(sizeof(ReadRequest));
            if (!readReq)
            {
                fprintf(stderr, "(liqsmt-to-read-request) Malloc failed \n");
                return NULL;
            }

            readReq->StartPosition = ctx->FileMd->Offset->Offsets[tIdx]->Offset;
            readReq->ReadSize = ctx->FileMd->Storage->Items[tIdx]->Count * ctx->FileMd->Storage->Items[tIdx]->RowSize;
        }
    }

    return readReq;
}

int SchemaValidateLiqsmt(Liqsmt *smt, TranslationCtx *ctx)
{
    if (!smt || !ctx)
    {
        fprintf(stderr, "(schema-validate-liqsmt) args invalid \n");
        return -1;
    }

    TableDefinition *tableDef = MatchTableDefFromLiqsmt(smt, ctx->FileMd->Schema);
    if (!tableDef)
    {
        fprintf(stderr, "(schema-validate-liqsmt) table not found in schema \n");
        return -1;
    }

    if (!smt->IsWildcardSelection)
    {
        for (int cIdx = 0; cIdx < smt->ColCount; cIdx++)
        {
            TableColDefinition *colDef = MatchColDefFromLiqsmt(smt->Columns[cIdx], tableDef);
            if (!colDef)
            {
                return -1;
            }
        }
    }

    return 0;
}

int ValidateLiqsmt(Liqsmt *smt)
{
    if (!smt)
    {
        fprintf(stderr, "(validate-liqsmt) smt is invalid \n");
        return -1;
    }

    if (!smt->TableName || strlen(smt->TableName) < 1)
    {
        fprintf(stderr, "(validate-liqsmt) table name is invalid \n");
        return -1;
    }

    if (!smt->Columns || smt->ColCount < 1 && !smt->IsWildcardSelection)
    {
        fprintf(stderr, "(validate-liqsmt) query is missing  \n");
        return -1;
    }

    return 0;
}