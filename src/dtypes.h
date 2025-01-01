#include <stdio.h>
#include <stdlib.h>

enum dtype { INTEGER, FLOAT, VARCHAR, DATE };

typedef struct {
    char *name;
    enum dtype dtype;
} Column;

typedef struct {
    char *tname;
    Column *cols;
} TableDef;
