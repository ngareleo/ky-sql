#ifndef METADATA_OFFSETS_H
#define METADATA_OFFSETS_H

#define OFFSET_TXT_WITH_PADDING 60

#ifndef MAX_TABLE_COUNT
#define MAX_TABLENAME_LENGTH 100
#endif

#ifndef MAX_TABLE_COUNT
#define MAX_TABLE_COUNT 100
#endif

typedef struct
{
    int Offset;      /** The offset of the file position from the start */
    char *TableName; /** The name of the table */
} TableOffset;

typedef struct
{
    TableOffset **Offsets; /** Offsets to tables */
    int OffsetCount;       /** Number of offsets */
    int ImwebOffset;       /** Offset to the immediate-write-buffer */
} Offset;

TableOffset *NewTableOffset(const char *, int);
int AddTableOffset(Offset *, const TableOffset *);
char *FormatTableOffset(const TableOffset *);
void FreeTableOffset(TableOffset *);

Offset *NewOffset(int, ...);
Offset *NewOffsetN(int, const TableOffset **);
void FreeOffset(Offset *);
char *FormatOffset(const TableOffset *);

#endif