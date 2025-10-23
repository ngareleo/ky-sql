#ifndef LANGUAGE_INSERT_H
#define LANGUAGE_INSERT_H

#include "Types/types.h"

typedef struct
{
    char *TableName; // The table being targeted
    DataBlock *Data; // The row data in order
} Linsmt;            // Language-Insert-Statement

Linsmt *CreateLangInsertStmt(char *, char **, char ***);
void *FreeLinsmt(Linsmt *);

#endif