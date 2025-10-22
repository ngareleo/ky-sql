#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "language.h"
#include "Utilities/utilities.h"

LanguageInsertStatement *CreateLanguageInsertStatement(char *tableName, int rowCount, char *columns, char **values)
{
    LanguageInsertStatement *statement;

    statement = malloc(sizeof(LanguageInsertStatement));
    if (!statement)
    {
        fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
        return NULL;
    }

    statement->Columns = malloc(strlen(columns) + 1);
    if (!statement->Columns)
    {
        free(statement);
        fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
        return NULL;
    }

    statement->TableName = malloc(sizeof(strlen(tableName) + 1));
    if (!statement->TableName)
    {
        free(statement->Columns);
        free(statement);
        fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
        return NULL;
    }

    statement->Rows = malloc(sizeof(char *) * rowCount);
    if (!statement->Rows)
    {
        free(statement->TableName);
        free(statement->Columns);
        free(statement);
        fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
        return NULL;
    }

    for (int rc = 0; rc < rowCount; rc++)
    {
        statement->Rows[rc] = malloc(strlen(rows[rc]) + 1);
        if (!statement->Rows[rc])
        {
            free(statement->TableName);
            free(statement->Columns);
            free(statement);

            for (int r_rc = 0; r_rc < rc; r_rc++)
            {
                free(statement->Rows[r_rc]);
            }

            fprintf(stderr, "(mock-book-table-insert-statement-err) malloc failed\n");
            return NULL;
        }

        strcpy(statement->Rows[rc], &rows[rc]);
    }
    strcpy(statement->Columns, &columns);
    strcpy(statement->TableName, &tableName);

    return statement;
}