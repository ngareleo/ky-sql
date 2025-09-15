typedef struct
{
    int SchemaId;             // The id of the schema we're targeting
    int TableId;              // The id of the table we're targeting
    struct                    //
    {                         //
        int ColumnId;         // Id of column to read
        char *PresentedAs;    // The name of the column in th result
    } *Columns;               // Column data
    struct                    //
    {                         //
        int *OrderByColumnId; // Order of the columns by ID
    } *Order;                 // Presentation order information
    struct                    //
    {                         //
        int ColumnCount;      // Number of column we're targetting
    } *Sizing;                // Size information
} LanguageQueryStatement;
