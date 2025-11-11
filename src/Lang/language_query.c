#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Utilities/utilities.h"
#include "include/language_query.h"

Liqsmt *CreateLiqsmt(char *tableName, char **columns, char **order, bool isWildcardSelection)
{
    Liqsmt *smt;
    Allocator *alloc = MallocInit();

    if (tableName && strlen(tableName) > 0)
    {
        smt->TableName = Malloc(strlen(tableName) + 1, alloc);
    }

    if (columns)
    {
        int colC = Count((void **)columns);
        smt->Columns = Malloc(sizeof(char *) * (colC + 1), alloc);
        for (int colIdx = 0; colIdx < colC; colIdx++)
        {
            smt->Columns[colIdx] = Malloc(strlen(columns[colIdx]) + 1, alloc);
        }
        smt->Columns[colC] = NULL;
    }

    if (order)
    {
        int orderC = Count((void **)order);
        smt->Order = Malloc(sizeof(char *) * (orderC + 1), alloc);
        for (int odx = 0; odx < order; odx++)
        {
            smt->Order[odx] = Malloc(strlen(order[odx]) + 1, alloc);
        }
        smt->Order[orderC] = NULL;
    }

    if (!VerifyAlloc(alloc))
    {
        free(smt->TableName);
        if (columns)
        {
            int colC = Count((void **)columns);
            for (int colIdx = 0; colIdx < colC; colIdx++)
            {
                free(smt->Columns[colIdx]);
            }
            free(smt->Columns);
        }

        if (order)
        {
            int orderC = Count((void **)order);
            for (int odx = 0; odx < orderC; odx++)
            {
                free(smt->Order[odx]);
            }
            free(smt->Order);
        }

        free(smt);
    }

    if (tableName && strlen(tableName) > 0)
    {
        strcpy(smt->TableName, tableName);
    }

    if (columns)
    {
        int colC = Count((void **)columns);
        smt->ColCount = colC;
        for (int cIdx = 0; cIdx < colC; cIdx++)
        {
            strcpy(smt->Columns[cIdx], columns[cIdx]);
        }
    }

    if (order)
    {
        int orderC = Count((void **)order);
        smt->OrderCount = orderC;
        for (int oIdx = 0; oIdx < order; oIdx++)
        {
            strcpy(smt->Order[oIdx], order[oIdx]);
        }
    }
    smt->IsWildcardSelection = isWildcardSelection;
    FreeAlloc(alloc);
    return smt;
}

void FreeLqsmt(Liqsmt *smt)
{
    if (smt)
    {
        if (smt->Order)
        {
            int oC = Count((void **)smt->Order);
            if (oC > 0)
            {
                for (int oIdx = 0; oIdx < oC; oIdx++)
                {
                    free(smt->Order[oIdx]);
                }
                free(smt->Order);
            }
        }

        if (smt->Columns)
        {
            int cC = Count((void **)smt->Columns);
            if (cC > 0)
            {
                for (int cIdx = 0; cIdx < cC; cIdx++)
                {
                    free(smt->Columns[cIdx]);
                }
                free(smt->Columns);
            }
        }

        free(smt);
    }
}