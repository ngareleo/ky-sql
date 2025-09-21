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
