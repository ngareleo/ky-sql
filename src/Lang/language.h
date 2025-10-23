#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "../Types/types.h"

typedef struct
{
    char *TableName;     // The table being targeted
    DataBlockType *Data; // The row data in order
} LangInsertStmt;

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
} LangQueryStmt;

LangInsertStmt *CreateLangInsertStmt(char *, char **, char ***);
#endif