#ifndef METADATA_SCHEMA_H
#define METADATA_SCHEMA_H

#include <stdbool.h>
#include <time.h>

#define MAX_TABLE_COLUMN_COUNT 100
#define MAX_TABLE_COUNT 100

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
    int Id;
    char *Name;
    enum SchemaType Type;
    bool IsPrimaryKey;
    bool IsNullable;
    bool IsUnique;
    void *DefaultValue;
    time_t CreatedAt;
    time_t LastModified;
} TableColDefinition;

typedef struct
{
    int Id;
    char *Name;
    TableColDefinition **Columns;
    int PrimaryKeyId;
    int ColumnCount;
    time_t CreatedAt;
    time_t LastModified;
} TableDefinition;

typedef struct
{
    TableDefinition **TableDefs;
    int TableCount;
    char *TagName;
    time_t CreatedAt;
    time_t LastModified;
} SchemaDefinition;

TableColDefinition *NewTableColumn(int, const char *, bool, enum SchemaType, bool, bool, const char *);
char *FormatTableColDefinition(TableColDefinition *);
void FreeTableColDefinition(TableColDefinition *);

TableDefinition *NewTableDefinition(char *, int, int, ...);
char *FormatTableDefinition(const TableDefinition *);
void FreeTableDefinition(TableDefinition *);

SchemaDefinition *NewSchemaDefinition(char *, ...);
void FreeSchemaDefinition(SchemaDefinition *);
char *FormatSchemaDefinition(const SchemaDefinition *);

#endif