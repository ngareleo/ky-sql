#include <stdio.h>
#include <stdlib.h>
#include "include/translation_context.h"
#include "../Lang/language.h"
#include "../Writer/writer.h"
#include "../Reader/reader.h"

int ValidateLanguageInsertStatement(LanguageInsertStatement *);

int LangInsertToWriteRequest(LanguageInsertStatement *statement, WriteRequest **request)
{
    if (ValidateLanguageInsertStatement(statement) != 0)
    {
        fprintf(stderr, "(language-insert-to-write-request) Args validation failed \n");
        return -1;
    }

    TranslationContext *context = GetTranslationContext();

    // Check that data is valid

    // First validate the statement matches the schema

    // Based on the schema create a single writable buffer

    // Create write request
}

int ValidateLanguageInsertStatement(LanguageInsertStatement *statement)
{
    if (!statement)
    {
        fprintf(stderr, "(validate-language-insert-statement) LanguageInsertStatement provided is null \n");
        return -1;
    }
    if (!statement->Columns)
    {
        fprintf(stderr, "(validate-language-insert-statement) LanguageInsertStatement.Columns provided is null \n");
        return -1;
    }

    if (!statement->Rows)
    {
        fprintf(stderr, "(validate-language-insert-statement) LanguageInsertStatement.Rows provided is null \n");
        return -1;
    }

    if (!statement->TableName)
    {
        fprintf(stderr, "(validate-language-insert-statement) LanguageInsertStatement.TableName provided is null \n");
        return -1;
    }

    return 0;
}