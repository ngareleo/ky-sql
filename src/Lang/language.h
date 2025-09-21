#ifndef LANGUAGE_H
#define LANGUAGE_H

typedef struct
{
    char *TableName;       // The table being targeted
    char *Columns;         // The columns in order
    char **Rows;           // The row data in order
} LanguageInsertStatement; //

typedef struct
{
    int SchemaId;             // The id of the schema we're targeting
    int TableId;              // The id of the table we're targeting
    struct                    //
    {                         //
        int ColumnId;         // Id of column to read
        char *PresentedAs;    // The name of the column in the result
    } *Columns;               //
    struct                    //
    {                         //
        int *OrderByColumnId; // Order of the columns by ID
    } *Order;                 //
    struct                    //
    {                         //
        int ColumnCount;      // Number of columns we're targeting
    } *Sizing;                //
} LanguageQueryStatement;

LanguageInsertStatement *CreateLanguageInsertStatement(char *tableName, int rowCount, char *columns, char **rows);
#endif