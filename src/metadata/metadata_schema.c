#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#define MAX_TABLE_COLUMN_COUNT 100
#define MAX_TABLE_COUNT 100

enum SchemaType
{
    BOOL,
    INTEGER,
    FLOAT,
    STRING,
    DATE,
};

struct TableColDefinition
{
    char *ColumnName;
    enum SchemaType ColumnType;
    bool ColumnIsUnique;
    void *ColumnDefaultValue;
    time_t ColumnCreatedAt;
    time_t ColumnLastModified;
};

struct TableColDefinition *NewTableColumn(char *name, enum SchemaType schemaType, bool isUnique, void *defaultValue)
{
    time_t now;
    struct TableColDefinition *colDef = malloc(sizeof(struct TableColDefinition));
    if (!name || !colDef || !defaultValue || !time(&now))
    {
        return NULL;
    }

    colDef->ColumnName = malloc(strlen(name) + 1);
    if (!colDef->ColumnName)
    {
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

    case INTEGER:

        colDef->ColumnDefaultValue = (int *)defaultValue; // Keeping things simple for now.

    case FLOAT:
        colDef->ColumnDefaultValue = (double *)defaultValue;

    case STRING:
        strcpy(colDef->ColumnDefaultValue, defaultValue);

    case DATE:
        colDef->ColumnDefaultValue = (time_t)defaultValue;
    }

    return colDef;
};

struct TableDefinition
{
    char *TableName;
    struct TableColDefinition **Columns;
    time_t CreatedAt;
    time_t LastModified;
};

struct TableDefinition *NewTableDefinition(char *name, ...)
{
    va_list args;
    time_t now;
    struct TableDefinition *tableDef;
    struct TableColDefinition
        **colDefs,
        *allCols[MAX_TABLE_COLUMN_COUNT],
        *currentCol;
    int count = 0;

    // Read all args into a buffer
    va_start(args, name);
    while ((currentCol = va_arg(args, struct TableColDefinition *)) != NULL)
    {
        allCols[count++] = currentCol;
    }
    va_end(args);

    tableDef = malloc(sizeof(struct TableColDefinition));
    if (!tableDef || !time(&now) || !name)
    {
        return NULL;
    }

    tableDef->TableName = malloc(sizeof(char *) * strlen(name));
    if (!tableDef->TableName)
    {
        free(tableDef);
        return NULL;
    }

    strcpy(tableDef->TableName, name);
    tableDef->Columns = malloc(sizeof(struct TableColDefinition *) * count);
    if (!tableDef->Columns)
    {
        free(tableDef->TableName);
        free(tableDef);
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        tableDef->Columns[c] = allCols[c];
    }

    tableDef->Columns = colDefs;
    tableDef->LastModified = now;
    tableDef->LastModified = now;

    return tableDef;
}

struct SchemaDefinition
{
    struct TableDefinition **TableDefs;
    char *TagName;
    time_t CreatedAt;
    time_t LastModified;
};

struct SchemaDefinition *NewSchemaDefinition(char *name, ...)
{
    va_list args;

    int count = 0;
    struct TableDefinition *allTables[MAX_TABLE_COUNT], *currentDef;
    va_start(args, name);

    while ((currentDef = va_arg(args, struct TableDefinition *)) != NULL)
    {
        allTables[count++] = currentDef;
    }
    va_end(args);

    struct SchemaDefinition *def = malloc(sizeof(struct SchemaDefinition));
    if (!def)
    {
        return NULL;
    }

    def->TableDefs = malloc(sizeof(struct TableDefinition) * count);
    if (!def->TableDefs)
    {
        free(def);
        return NULL;
    }

    for (int c = 0; c < count; c++)
    {
        def->TableDefs[c] = allTables[c];
    }

    return def;
}

int main()
{
    float duration = 3.2;
    struct TableColDefinition *nameCol = NewTableColumn("Name", STRING, false, NULL);
    struct TableColDefinition *durationCol = NewTableColumn("Duration", FLOAT, false, &duration);
    struct TableColDefinition *relaseDateCol = NewTableColumn("Release Date", DATE, false, NULL);
    struct TableDefinition *songTableDef = NewTableDefinition("Songs", nameCol, durationCol, relaseDateCol);
    struct SchemaDefinition *music = NewSchemaDefinition(songTableDef);
}