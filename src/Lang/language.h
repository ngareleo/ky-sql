#ifndef LANGUAGE_H
#define LANGUAGE_H

typedef struct
{
    int SchemaId;     // The schema we're targeting
    int TableId;      // The table being targeted
    struct            //
    {                 //
        int ColumnId; // The id of the column we're inserting information
        char **Data;  // The data we're supposed to write
    } *Columns;       //
} LanguageInsertStatement;

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

#endif