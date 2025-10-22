#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "language.h"
#include "Utilities/utilities.h"
#include "Types/types.h"

LanguageInsertStatement *CreateLanguageInsertStatement(char *tableName, char **columns, char ***values)
{
    LanguageInsertStatement *statement;

    statement = malloc(sizeof(LanguageInsertStatement));
    if (!statement)
    {
        fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
        return NULL;
    }

    statement->TableName = malloc(sizeof(strlen(tableName) + 1));
    if (!statement->TableName)
    {
        free(statement);
        fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
        return NULL;
    }
    strcpy(statement->TableName, &tableName);
    statement->Data = CreateDataBlock(columns, values);
    if (!statement->Data)
    {
        free(statement->TableName);
        free(statement);
        fprintf(stderr, "(create-language-insert-statement) failed to create data block \n");
        return NULL;
    }
    return statement;
}