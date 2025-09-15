typedef struct
{
    int SchemaId;             /* The id of the schema we're targeting */
    int TableId;              /* The id of the table we're targeting */
    struct                    /* Columns struct */
    {                         /* Columns struct body */
        int ColumnId;         /* Id of column to read */
        char *PresentedAs;    /* The name of the column in the result */
    } *Columns;               /* Column data */
    struct                    /* Order struct */
    {                         /* Order struct body */
        int *OrderByColumnId; /* Order of the columns by ID */
    } *Order;                 /* Presentation order information */
    struct                    /* Sizing struct */
    {                         /* Sizing struct body */
        int ColumnCount;      /* Number of columns we're targeting */
    } *Sizing;                /* Size information */
} LanguageQueryStatement;
