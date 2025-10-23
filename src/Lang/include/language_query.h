#ifndef LANGUAGE_QUERY_H
#define LANGUAGE_QUERY_H

#include <stdbool.h>

typedef struct
{
    char *TableName;          // Target table name
    char **Columns;           // Selected columns
    int **ColCount;           // Number of queried columns
    char **Order;             // Column order
    int **OrderCount;         // Number of columns ordered
    bool IsWildcardSelection; // "select *"
} Liqsmt;                     // Language-Query-Statement

Liqsmt *CreateLqsmt(char *tableName, char **columns, char **order, bool isWildcardSelection);
void FreeLqsmt(Liqsmt *);

#endif