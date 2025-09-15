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
    int Offset; /** The offset of the file position from the start */
    int Id;     /** The id of the table */
} TableOffset;

typedef struct
{
    TableOffset **Offsets; /** Offsets to tables */
    int TableCount;        /** Number of offsets */
    int ImwebOffset;       /** Offset to the immediate-write-buffer */
} Offset;

TableOffset *NewTableOffset(int, int);
int AddTableOffset(Offset *, const TableOffset *);
char *FormatTableOffset(const TableOffset *);

Offset *NewOffset(int, ...);
Offset *NewOffsetN(int, const TableOffset **);
void FreeOffset(Offset *);
char *FormatOffset(const Offset *);

#endif