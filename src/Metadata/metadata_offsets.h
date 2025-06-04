#ifndef METADATA_OFFSETS_H
#define METADATA_OFFSETS_H

#define OFFSET_TXT_WITH_PADDING 60
#define MAX_TABLENAME_LENGTH 100
#define MAX_TABLE_COUNT 100

struct TableOffset
{
    int Offset;      /** The offset of the file position from 0 */
    char *TableName; /** The name of the table */
};

struct Offset
{
    struct TableOffset **Offsets; /** Offsets to tables */
    int OffsetCount;              /** Number of offsets */
    int ImwebOffset;              /** Offset to the immediate-write-buffer */
};

struct TableOffset *NewTableOffset(const char *, int);
struct Offset *NewOffset(int, ...);
struct Offset *NewOffsetN(int, const struct TableOffset **);
int AddTableOffset(struct Offset *, const struct TableOffset *);
void FreeOffset(struct Offset *);
char *FormatTableOffset(const struct TableOffset *);

#endif