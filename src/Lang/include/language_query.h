#ifndef LANGUAGE_QUERY_H
#define LANGUAGE_QUERY_H

#include <stdbool.h>

typedef struct
{
    bool IsWildcardSelection; /** The query requests all data using '*' */
    int ColCount;             /** Number of queried columns */
    int OrderCount;           /** Number of columns ordered */
    int Limit;                /** The query has a limit. A value of 0 means no limit */
    char *TableName;          /** Target table name */
    char **Columns;           /** Selected columns */
    char **Order;             /** Column order */

} Liqsmt; /** Language-Query-Statement */

Liqsmt *CreateLiqsmt(char *tableName, char **columns, char **order, bool isWildcardSelection);
void FreeLqsmt(Liqsmt *);

#endif