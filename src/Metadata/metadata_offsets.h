#ifndef METADATA_OFFSETS_H
#define METADATA_OFFSETS_H

#define OFFSET_TXT_WITH_PADDING 60
#define MAX_TABLENAME_LENGTH 100
#define MAX_TABLE_COUNT 100

typedef struct
{
    int Offset;      /** The offset of the file position from 0 */
    char *TableName; /** The name of the table */
} TableOffset;

typedef struct
{
    TableOffset **Offsets; /** Offsets to tables */
    int OffsetCount;       /** Number of offsets */
    int ImwebOffset;       /** Offset to the immediate-write-buffer */
} Offset;

TableOffset *NewTableOffset(const char *, int);
Offset *NewOffset(int, ...);
Offset *NewOffsetN(int, const TableOffset **);
int AddTableOffset(struct Offset *, const TableOffset *);
void FreeOffset(Offset *);
char *FormatTableOffset(const TableOffset *);

#endif