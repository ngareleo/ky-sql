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

typedef struct
{
    char *ColumnName;
    enum SchemaType ColumnType;
    bool ColumnIsUnique;
    void *ColumnDefaultValue;
    time_t ColumnCreatedAt;
    time_t ColumnLastModified;
} TableColDefinition;

typedef struct
{
    char *TableName;
    struct TableColDefinition **Columns;
    int ColumnCount;
    time_t CreatedAt;
    time_t LastModified;
} TableDefinition;

typedef struct
{
    struct TableDefinition **TableDefs;
    int TableCount;
    char *TagName;
    time_t CreatedAt;
    time_t LastModified;
} SchemaDefinition;

TableColDefinition *NewTableColumn(char *, enum SchemaType, bool, const void *);
char *FormatTableColDefinition(TableColDefinition *);
void FreeTableColDefinition(TableColDefinition *);

TableDefinition *NewTableDefinition(char *, ...);
char *FormatTableDefinition(TableDefinition *);
void FreeTableDefinition(TableDefinition *);

SchemaDefinition *NewSchemaDefinition(char *, ...);
void IntrospectSchema(SchemaDefinition *);
void FreeSchemaDefinition(SchemaDefinition *);

#endif