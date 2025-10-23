#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/translation_context.h"
#include "Metadata/metadata.h"
#include "Lang/language.h"
#include "Reader/reader.h"
#include "Types/types.h"
#include "Utilities/utilities.h"
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
    if (!req)
    {
        fprintf(stderr, "(linsmt-to-write-request) failed to create write request \n");
        return -1;
    }

    return req;
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
        if (strcmp(name, def->Columns[col]->Name))
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
    for (int c = 0; c < def->TableCount; c++)
    {
        if (strcmp(def->TableDefs[c]->Name, stmt->TableName))
        {
            tableDef = def->TableDefs[c];
        }
    }

    return tableDef;
}

WriteRequest *CreateWriteRequest(Linsmt *stmt, TranslationContext *ctx, char *writable)
{
    if (!stmt || !writable || !ctx)
    {
        fprintf(stderr, "(create-write-request) args are invalid \n");
        return -1;
    }

    WriteRequest *req;
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

    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, ctx->FileMetadataObj->Schema);
    // Here we read the table offset and write there
    for (int c = 0; c < ctx->FileMetadataObj->Offset->TableCount; c++)
    {
        if (tableDef->Id == ctx->FileMetadataObj->Offset->Offsets[c]->Id)
        {
            TableOffset *off = ctx->FileMetadataObj->Offset->Offsets[c]->Offset;
            req->StartPosition = off->Offset;
        }
    }

    return req;
}

char *TranslateLinsmt(Linsmt *stmt, TranslationContext *ctx)
{
    FILE *stream;
    char *buffer, **columns;
    size_t size;

    SchemaDefinition *schemaDef = ctx->FileMetadataObj->Schema;
    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, schemaDef);

    if (!tableDef)
    {
        return -1;
    }

    columns = stmt->Data->Header;

    if (!stmt || !ctx)
    {
        fprintf(stderr, "(translate-linsmt) args are invalid \n");
        return -1;
    }

    stream = open_memstream(&buffer, &size);
    if (!stream)
    {
        fprintf(stderr, "(translate-linsmt) open_memstream failed.\n");
        return NULL;
    }

    for (int c = 0; c < stmt->Data->Size->HeaderCount; c++)
    {
        for (int c_1 = 0; c_1 < tableDef->ColumnCount; c_1++)
        {
            TableColDefinition *colDef = MatchTableColFromLinsmt(columns[c_1], tableDef);
            if (colDef)
            {
                switch (colDef->Type)
                {
                case ID:
                    break;
                case BOOL:
                    break;
                case INTEGER:
                    break;
                case FLOAT:
                    break;
                case STRING:
                    break;
                case DATE:
                    break;
                default:
                    break;
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
    if (!ctx)
    {
        fprintf(stderr, "(schema-validate-linsmt) context is invalid \n");
        return -1;
    }

    bool colsFound = false;
    char **rowNames = stmt->Data->Header, **schemaColNames;
    SchemaDefinition *schema = ctx->FileMetadataObj->Schema;
    TableDefinition *tableDef = MatchTableDefFromLinsmt(stmt, schema);

    if (!tableDef)
    {
        return -1;
    }

    schemaColNames = malloc(sizeof(char *) * ((tableDef->ColumnCount) + 1));
    schemaColNames[tableDef->ColumnCount] = NULL;

    for (int stmtC = 0; stmtC < stmt->Data->Size->HeaderCount; stmtC++)
    {
        TableColDefinition *colDef = MatchTableColFromLinsmt(stmt->Data->Header[stmtC], tableDef);
        if (!colDef)
        {
            colsFound = true;
        }
    }

    return colsFound ? 0 : -1;
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