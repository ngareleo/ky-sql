#include <stdio.h>
#include <stdlib.h>
#include "include/translation_context.h"
#include "Lang/language.h"
#include "Reader/reader.h"
#include "Types/types.h"
#include "Writer/writer.h"

int LangInsertStmtToWriteRequest(LangInsertStmt *stmt, WriteRequest **req)
{
    if (ValidateLangInsertStmt(stmt) != 0)
    {
        fprintf(stderr, "(lang-insert-stmt-to-write-request) statement failed validation \n");
        return -1;
    }

    char *writable;
    WriteRequest *req;
    TranslationContext *ctx = GetTranslationContext();

    if (!ctx)
    {
        fprintf(stderr, "(lang-insert-stmt-to-write-request) context unavailable \n");
        return -1;
    }

    if (SchemaValidateInsertStmt(stmt, ctx) != 0)
    {
        fprintf(stderr, "(lang-insert-stmt-to-write-request) statement failed schema validation \n");
        return -1;
    }

    writable = TranslateInsertStmtToWritable(stmt, ctx);
    if (!writable)
    {
        fprintf(stderr, "(lang-insert-stmt-to-write-request) couldn't generate a writable \n");
        return -1;
    }

    req = CreateWriteRequest(stmt, ctx, writable);
    return req;
}

WriteRequest *CreateWriteRequest(LangInsertStmt *stmt, TranslationContext *ctx, char *writable) {}

int SchemaValidateInsertStmt(LangInsertStmt *stmt, TranslationContext *ctx) {}

int TranslateInsertStmtToWritable(LangInsertStmt *stmt, TranslationContext *ctx) {}

int ValidateLangInsertStmt(LangInsertStmt *stmt)
{
    if (!stmt)
    {
        fprintf(stderr, "(validate-lang-insert-stmt) statment provided is null \n");
        return -1;
    }

    if (!stmt->Data || !stmt->Data->IsValid)
    {
        fprintf(stderr, "(validate-lang-insert-stmt) statement is invalid \n");
        return -1;
    }

    if (!stmt->Data->IsEmpty)
    {
        fprintf(stderr, "(validate-lang-insert-stmt) statement is empty  \n");
        return -1;
    }

    if (!stmt->TableName)
    {
        fprintf(stderr, "(validate-lang-insert-stmt) tablename is invalid \n");
        return -1;
    }

    return 0;
}