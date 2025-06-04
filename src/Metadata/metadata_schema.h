#ifndef METADATA_SCHEMA_H
#define METADATA_SCHEMA_H

#include <stdbool.h>
#include <time.h>

#define MAX_TABLE_COLUMN_COUNT 100
#define MAX_TABLE_COUNT 100
#define MAX_LOG_BUFFER 10000

enum SchemaType
{
    ID,
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

struct TableDefinition
{
    char *TableName;
    struct TableColDefinition **Columns;
    int ColumnCount;
    time_t CreatedAt;
    time_t LastModified;
};

struct SchemaDefinition
{
    struct TableDefinition **TableDefs;
    int TableCount;
    char *TagName;
    time_t CreatedAt;
    time_t LastModified;
};

struct TableColDefinition *NewTableColumn(char *, enum SchemaType, bool, const void *);
char *FormatTableColDefinition(struct TableColDefinition *);
void FreeTableColDefinition(struct TableColDefinition *);

struct TableDefinition *NewTableDefinition(char *, ...);
char *FormatTableDefinition(struct TableDefinition *);
void FreeTableDefinition(struct TableDefinition *);

struct SchemaDefinition *NewSchemaDefinition(char *, ...);
void IntrospectSchema(struct SchemaDefinition *);
void FreeSchemaDefinition(struct SchemaDefinition *);

#endif