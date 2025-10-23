#include <stdio.h>
#include <stdlib.h>
#include "include/translation_context.h"
#include "Lang/language.h"
#include "Reader/reader.h"
#include "Types/types.h"
#include "Writer/writer.h"

int LinsmtToWriteRequest(Linsmt *stmt, WriteRequest **req)
{

    char *writable;
    WriteRequest *req;
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

    writable = TranslateLinsmt(stmt, ctx);
    if (!writable)
    {
        fprintf(stderr, "(linsmt-to-write-request) couldn't generate a writable \n");
        return -1;
    }

    req = CreateWriteRequest(stmt, ctx, writable);
    return req;
}

int SchemaValidateLinsmt(Linsmt *stmt, TranslationContext *ctx) {}

WriteRequest *CreateWriteRequest(Linsmt *stmt, TranslationContext *ctx, char *writable) {}

int TranslateLinsmt(Linsmt *stmt, TranslationContext *ctx) {}

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