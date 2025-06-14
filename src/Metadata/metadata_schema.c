#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "metadata_schema.h"

TableColDefinition *NewTableColumn(char *name, enum SchemaType schemaType, bool isUnique, const void *defaultValue)
{
    time_t now;
    TableColDefinition *colDef = malloc(sizeof(TableColDefinition));
    if (!name || !colDef || !defaultValue)
    {
        fprintf(stderr, "Error creating column. Passed null values. \n");
        return NULL;
    }

    if (!time(&now))
    {
        fprintf(stderr, "Error creating column. time failed. \n");
        return NULL;
    }

    colDef->ColumnName = malloc(strlen(name) + 1);
    if (!colDef->ColumnName)
    {
        fprintf(stderr, "Error creating column. malloc failed. \n");
        return NULL;
    }
    strcpy(colDef->ColumnName, name);

    colDef->ColumnCreatedAt = now;
    colDef->ColumnLastModified = now;
    colDef->ColumnType = schemaType;
    colDef->ColumnIsUnique = isUnique;

    switch (schemaType)
    {
    case BOOL:
        colDef->ColumnDefaultValue = (bool *)defaultValue;
        break;

    case INTEGER:
        colDef->ColumnDefaultValue = (int *)defaultValue; // Keeping things simple for now.
        break;

    case FLOAT:
        colDef->ColumnDefaultValue = (double *)defaultValue;
        break;

    case STRING:
        strcpy(colDef->ColumnDefaultValue, defaultValue);
        break;

    case DATE:
        colDef->ColumnDefaultValue = (time_t *)defaultValue;
        break;

    case ID:
        colDef->ColumnDefaultValue = (long)defaultValue; // ID Generation kicks in here
        break;
    }

    return colDef;
};

void FreeTableColDefinition(TableColDefinition *def)
{
    if (!def)
    {
        return;
    }

    free(def->ColumnName);
    free(def);
}

TableDefinition *NewTableDefinition(char *name, ...)
{
    va_list args;
    time_t now;
    TableDefinition *tableDef;
    TableColDefinition
        **colDefs,
        *allCols[MAX_TABLE_COLUMN_COUNT],
        *currCol;
    int count = 0;

    if (!name)
    {
        fprintf(stderr, "[NewTableDefinition] Error creating Table. name is NULL. \n");
        return NULL;
    }

    if (!time(&now))
    {
        fprintf(stderr, "[NewTableDefinition] Error creating Table. time failed. \n");
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
        fprintf(stderr, "[NewTableDefinition] Error creating Table. malloc failed. \n");
        return NULL;
    }

    tableDef->TableName = malloc(strlen(name) + 1);
    if (!tableDef->TableName)
    {
        fprintf(stderr, "[NewTableDefinition] Error creating Table. malloc failed. \n");
        free(tableDef);
        return NULL;
    }
    strcpy(tableDef->TableName, name);

    tableDef->Columns = malloc(sizeof(TableColDefinition *) * count);
    if (!tableDef->Columns)
    {
        fprintf(stderr, "[NewTableDefinition] Error creating Table. malloc failed. \n");
        free(tableDef->TableName);
        free(tableDef);
        return NULL;
    }
    for (int c = 0; c < count; c++)
    {
        tableDef->Columns[c] = allCols[c];
    }

    tableDef->ColumnCount = count;
    tableDef->Columns = colDefs;
    tableDef->LastModified = now;
    tableDef->LastModified = now;

    return tableDef;
}

const int PADDING = 100;

char *FormatTableDefinition(TableDefinition *def)
{
    char *buffer;
    size_t size;
    FILE *memstream;

    if (!def)
    {
        sprintf(stderr, "[FormatTableDefinition] Error formating table: Cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        sprintf(stderr, "[FormatTableDefinition] Error formating table: open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "name          = %s\n", def->TableName);
    fprintf(memstream, "created       = %s\n", def->CreatedAt);
    fprintf(memstream, "last-modified = %s\n", def->LastModified);
    fprintf(memstream, "column-count  = %s\n", def->ColumnCount);
    fprintf(memstream, "columns       = \n", def->Columns);

    for (int c = 0; c < CountColumns(def); c++)
    {
        sprintf(memstream, "\t%s\n", FormatTableColDefinition(def->Columns[c]));
    }

    return buffer;
}

void FreeTableDefinition(TableDefinition *def)
{
    if (!def)
    {
        return;
    }

    free(def->TableName);
    for (int c = 0; c < def->ColumnCount; c++)
    {
        FreeTableColDefinition(def->Columns[c]);
    }
    free(def->Columns);
    free(def);
}

SchemaDefinition *NewSchemaDefinition(char *name, ...)
{
    va_list args;
    int count = 0;
    TableDefinition *allTables[MAX_TABLE_COUNT], *currentDef;
    SchemaDefinition *def;

    if (!name)
    {
        fprintf(stderr, "[NewSchemaDefinition] Error creating schema definition. name is null\n");
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
        fprintf(stderr, "[NewSchemaDefinition] Error creating schema definition. malloc failed\n");
        return NULL;
    }

    def->TableDefs = malloc(sizeof(TableDefinition) * count);
    if (!def->TableDefs)
    {
        fprintf(stderr, "[NewSchemaDefinition] Error creating schema definition. malloc failed\n");
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
    if (!def)
    {
        return;
    }

    free(def->TagName);
    for (int c = 0; c < def->TableCount; c++)
    {
        FreeTableDefinition(def->TableDefs[c]);
    }
    free(def->TableDefs);
    free(def);
}
