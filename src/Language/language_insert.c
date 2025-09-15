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
