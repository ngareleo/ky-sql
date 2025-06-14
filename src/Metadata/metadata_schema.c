#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "metadata_schema.h"

TableColDefinition *NewTableColumn(const char *name, enum SchemaType schemaType, bool isUnique, const char *defaultValue)
{
    time_t now;
    TableColDefinition *colDef;

    if (!name || !defaultValue)
    {
        fprintf(stderr, "(new-table-column-err) Passed null values. \n");
        return NULL;
    }

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(new-table-column-err) time failed. \n");
        return NULL;
    }

    colDef = malloc(sizeof(TableColDefinition));
    if (!colDef)
    {
        fprintf(stderr, "(new-table-column-err) Passed null values. \n");
        return NULL;
    }

    colDef->ColumnName = malloc(strlen(name) + 1);
    if (!colDef->ColumnName)
    {
        free(colDef);
        fprintf(stderr, "(new-table-column-err) malloc failed. \n");
        return NULL;
    }

    colDef->ColumnDefaultValue = malloc(strlen(defaultValue) + 1);
    if (!colDef->ColumnDefaultValue)
    {
        free(colDef->ColumnName);
        free(colDef);
        fprintf(stderr, "(new-table-column-err) malloc failed. \n");
        return NULL;
    }

    strcpy(colDef->ColumnDefaultValue, defaultValue);
    strcpy(colDef->ColumnName, name);
    colDef->ColumnCreatedAt = now;
    colDef->ColumnLastModified = now;
    colDef->ColumnType = schemaType;
    colDef->ColumnIsUnique = isUnique;
    colDef->ColumnName = malloc(strlen(name) + 1);

    return colDef;
};

void FreeTableColDefinition(TableColDefinition *def)
{
    if (def)
    {
        free(def->ColumnName);
        free(def);
    }
}

TableDefinition *NewTableDefinition(char *name, ...)
{
    va_list args;
    time_t now;
    TableDefinition *tableDef;
    TableColDefinition **colDefs, *allCols[MAX_TABLE_COLUMN_COUNT], *currCol;
    int count = 0;

    if (!name)
    {
        fprintf(stderr, "(new-table-defintion-err) name is NULL. \n");
        return NULL;
    }

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(new-table-defintion-err) time failed. \n");
        return NULL;
    }

    va_start(args, name);
    while ((currCol = va_arg(args, TableColDefinition *)) != NULL)
    {
        allCols[count++] = currCol;
    }
    va_end(args);

    tableDef = malloc(sizeof(TableColDefinition));
    if (!tableDef)
    {
        fprintf(stderr, "(new-table-defintion-err) malloc failed. \n");
        return NULL;
    }

    tableDef->TableName = malloc(strlen(name) + 1);
    if (!tableDef->TableName)
    {
        fprintf(stderr, "(new-table-defintion-err) malloc failed. \n");
        free(tableDef);
        return NULL;
    }

    tableDef->Columns = malloc(sizeof(TableColDefinition *) * count);
    if (!tableDef->Columns)
    {
        fprintf(stderr, "(new-table-defintion-err) malloc failed. \n");
        free(tableDef->TableName);
        free(tableDef);
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        tableDef->Columns[c] = allCols[c];
    }

    strcpy(tableDef->TableName, name);
    tableDef->ColumnCount = count;
    tableDef->Columns = colDefs;
    tableDef->LastModified = now;
    tableDef->LastModified = now;

    return tableDef;
}

char *FormatSchemaDefinition(const SchemaDefinition *def)
{
    char *buffer;
    FILE *memstream;
    size_t size;

    if (!def)
    {
        sprintf(stderr, "(format-table-schema-err) cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        sprintf(stderr, "(format-table-schema-err) open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "(log) schema-tag-name      = %s\n", def->TagName);
    fprintf(memstream, "(log) schema-date-created  = %s\n", def->CreatedAt);
    fprintf(memstream, "(log) schema-last-modified = %s\n", def->LastModified);
    fprintf(memstream, "(log) schema-table-count   = %s\n", def->TableCount);

    for (int c = 0; c < CountColumns(def); c++)
    {
        sprintf(memstream,
                "(log) schema-table-definition['%s'] = %s\n",
                def->TableDefs[c]->TableName,
                FormatTableDefinition(def->TableDefs[c]));
    }

    fclose(memstream);
    return buffer;
}

char *FormatTableDefinition(const TableDefinition *def)
{
    char *buffer;
    FILE *memstream;
    size_t size;

    if (!def)
    {
        sprintf(stderr, "(format-table-definition-err) Cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        sprintf(stderr, "(format-table-definition-err) open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "(log) table-name          = %s\n", def->TableName);
    fprintf(memstream, "(log) table-created       = %ld\n", def->CreatedAt);
    fprintf(memstream, "(log) table-last-modified = %ld\n", def->LastModified);
    fprintf(memstream, "(log) table-column-count  = %s\n", def->ColumnCount);

    for (int c = 0; c < CountColumns(def); c++)
    {
        sprintf(memstream,
                "(log) table-column['%s'] = %s\n",
                def->Columns[c]->ColumnName,
                FormatTableColDefinition(def->Columns[c]));
    }

    fclose(memstream);
    return buffer;
}

char *FormatColDefinition(TableColDefinition *def)
{
    char *buffer;
    FILE *memstream;
    size_t size;

    if (!def)
    {
        sprintf(stderr, "(format-table-col-definition-err) Cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        sprintf(stderr, "(format-table-col-definition-err) open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "(log) column-name          = %s\n", def->ColumnName);
    fprintf(memstream, "(log) column-created       = %ld\n", def->ColumnCreatedAt);
    fprintf(memstream, "(log) column-last-modified = %ld\n", def->ColumnLastModified);
    fprintf(memstream, "(log) column-default       = %d\n", def->ColumnDefaultValue);
    fprintf(memstream, "(log) column-is-unique     = %d\n", def->ColumnIsUnique);
    fprintf(memstream, "(log) column-type          = %d\n", def->ColumnType);

    fclose(memstream);
    return buffer;
}

void FreeTableDefinition(TableDefinition *def)
{
    if (def)
    {
        free(def->TableName);
        for (int c = 0; c < def->ColumnCount; c++)
        {
            FreeTableColDefinition(def->Columns[c]);
        }
        free(def->Columns);
        free(def);
    }
}

SchemaDefinition *NewSchemaDefinition(char *name, ...)
{
    va_list args;
    int count = 0;
    TableDefinition *allTables[MAX_TABLE_COUNT], *currentDef;
    SchemaDefinition *def;

    if (!name)
    {
        fprintf(stderr, "(new-schema-definition-err) name is null\n");
        return NULL;
    }

    va_start(args, name);
    while ((currentDef = va_arg(args, TableDefinition *)) != NULL)
    {
        allTables[count++] = currentDef;
    }
    va_end(args);

    def = malloc(sizeof(SchemaDefinition));
    if (!def)
    {
        fprintf(stderr, "(new-schema-definition-err) malloc failed\n");
        return NULL;
    }

    def->TableDefs = malloc(sizeof(TableDefinition) * count);
    if (!def->TableDefs)
    {
        fprintf(stderr, "(new-schema-definition-err) malloc failed\n");
        free(def);
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        def->TableDefs[c] = allTables[c];
    }

    def->TableCount = count;

    return def;
}

void FreeSchemaDefinition(SchemaDefinition *def)
{
    if (def)
    {
        free(def->TagName);
        for (int c = 0; c < def->TableCount; c++)
        {
            FreeTableDefinition(def->TableDefs[c]);
        }
        free(def->TableDefs);
        free(def);
    }
}
