#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "language.h"
#include "Utilities/utilities.h"
#include "Types/types.h"

Linsmt *CreateLangInsertStmt(char *tableName, char **columns, char ***values)
{
    Linsmt *stmt;
    Allocator *alloc = MallocInit();

    stmt = Malloc(sizeof(Linsmt), alloc);
    if (stmt)
    {
        stmt->TableName = Malloc(sizeof(strlen(tableName) + 1), alloc);
    }

    stmt->Data = CreateDataBlock(columns, values);

    if (!VerifyAlloc(alloc) ||
        !stmt->Data)
    {
        free(stmt->TableName);
        free(stmt);
        fprintf(stderr, "(create-lang-insert-stmt) failed to mem alloc \n");
        return NULL;
    }

    strcpy(stmt->TableName, &tableName);
    FreeAlloc(alloc);
    return stmt;
}