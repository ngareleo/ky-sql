#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "metadata_schema.h"

TableColDefinition *NewTableColumn(
    int id,
    const char *name,
    bool isPrimaryKey,
    enum SchemaType schemaType,
    bool isNullable,
    bool isUnique,
    const char *defaultValue)
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

    colDef->Name = malloc(strlen(name) + 1);
    if (!colDef->Name)
    {
        free(colDef);
        fprintf(stderr, "(new-table-column-err) malloc failed. \n");
        return NULL;
    }

    colDef->DefaultValue = malloc(strlen(defaultValue) + 1);
    if (!colDef->DefaultValue)
    {
        free(colDef->Name);
        free(colDef);
        fprintf(stderr, "(new-table-column-err) malloc failed. \n");
        return NULL;
    }

    strcpy(colDef->Name, name);
    strcpy(colDef->DefaultValue, defaultValue);
    colDef->Id = id;
    colDef->CreatedAt = now;
    colDef->LastModified = now;
    colDef->Type = schemaType;
    colDef->IsUnique = isUnique;
    colDef->IsNullable = isNullable;
    colDef->IsPrimaryKey = isPrimaryKey;

    return colDef;
};

void FreeTableColDefinition(TableColDefinition *def)
{
    if (def)
    {
        free(def->Name);
        free(def);
    }
}

TableDefinition *NewTableDefinition(char *name, int id, int primaryKeyId, ...)
{
    va_list args;
    time_t now;
    TableDefinition *tableDef;
    TableColDefinition *allCols[MAX_TABLE_COLUMN_COUNT], *currCol;
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

    /**
     * Remember to update this whenever the function def changes
     */
    va_start(args, primaryKeyId);
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

    tableDef->Name = malloc(strlen(name) + 1);
    if (!tableDef->Name)
    {
        fprintf(stderr, "(new-table-defintion-err) malloc failed. \n");
        free(tableDef);
        return NULL;
    }

    tableDef->Columns = malloc(sizeof(TableColDefinition *) * count);
    if (!tableDef->Columns)
    {
        fprintf(stderr, "(new-table-defintion-err) malloc failed. \n");
        free(tableDef->Name);
        free(tableDef);
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        tableDef->Columns[c] = allCols[c];
    }

    strcpy(tableDef->Name, name);
    tableDef->Id = id;
    tableDef->PrimaryKeyId = primaryKeyId;
    tableDef->ColumnCount = count;
    tableDef->LastModified = now;
    tableDef->CreatedAt = now;
    return tableDef;
}

char *FormatSchemaDefinition(const SchemaDefinition *def)
{
    char *buffer;
    FILE *memstream;
    size_t size;

    if (!def)
    {
        fprintf(stderr, "(format-table-schema-err) cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        fprintf(stderr, "(format-table-schema-err) open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "(log) schema-tag-name      = %s\n", def->TagName);
    fprintf(memstream, "(log) schema-date-created  = %ld\n", def->CreatedAt);
    fprintf(memstream, "(log) schema-last-modified = %ld\n", def->LastModified);
    fprintf(memstream, "(log) schema-table-count   = %d\n", def->TableCount);

    for (int c = 0; c < def->TableCount; c++)
    {
        fprintf(memstream,
                "(log) schema-table-definition['%s'] = %s\n",
                def->TableDefs[c]->Name,
                FormatTableDefinition(def->TableDefs[c]));
    }

    fclose(memstream);
    return buffer;
}

char *GetColumnNameById(TableDefinition *def, int id)
{
    if (!def)
    {
        fprintf(stderr, "(get-table-name-by-id-err) `def` is null");
        return NULL;
    }

    for (int ti = 0; ti < def->ColumnCount; ti++)
    {
        if (def->Columns[ti]->Id == id)
        {
            return def->Columns[ti]->Name;
        }
    }

    return NULL;
}

char *FormatTableDefinition(const TableDefinition *def)
{
    char *buffer;
    FILE *memstream;
    size_t size;

    if (!def)
    {
        fprintf(stderr, "(format-table-definition-err) Cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        fprintf(stderr, "(format-table-definition-err) open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "(log) table-id            = %d\n", def->Id);
    fprintf(memstream, "(log) table-name          = %s\n", def->Name);
    fprintf(memstream, "(log) table-primary-key   = %s\n", GetColumnNameById((TableDefinition *)def, def->PrimaryKeyId));
    fprintf(memstream, "(log) table-column-count  = %d\n", def->ColumnCount);
    fprintf(memstream, "(log) table-created       = %ld\n", def->CreatedAt);
    fprintf(memstream, "(log) table-last-modified = %ld\n", def->LastModified);

    for (int c = 0; c < def->ColumnCount; c++)
    {
        fprintf(memstream,
                "(log) table-column['%s'] = %s\n",
                def->Columns[c]->Name,
                FormatTableColDefinition(def->Columns[c]));
    }

    fclose(memstream);
    return buffer;
}

char *FormatTableColDefinition(TableColDefinition *def)
{
    char *buffer;
    FILE *memstream;
    size_t size;

    if (!def)
    {
        fprintf(stderr, "(format-table-col-definition-err) Cannot format a null value");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        fprintf(stderr, "(format-table-col-definition-err) open_memstream failed");
        return NULL;
    }

    fprintf(memstream, "(log) column-id             = %d\n", def->Id);
    fprintf(memstream, "(log) column-name           = %s\n", def->Name);
    fprintf(memstream, "(log) column-is-primary-key = %d\n", def->IsPrimaryKey);
    fprintf(memstream, "(log) column-is-nullable    = %d\n", def->IsNullable);
    fprintf(memstream, "(log) column-is-unique      = %d\n", def->IsNullable);
    fprintf(memstream, "(log) column-default-value  = %s\n", (char *)def->DefaultValue);
    fprintf(memstream, "(log) column-created        = %ld\n", def->CreatedAt);
    fprintf(memstream, "(log) column-last-modified  = %ld\n", def->LastModified);

    fclose(memstream);
    return buffer;
}

void FreeTableDefinition(TableDefinition *def)
{
    if (def)
    {
        free(def->Name);
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
