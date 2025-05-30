#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define OFFSET_TXT_WITH_PADDING 60

#pragma pack(1)
struct TableOffset
{
    char *TableName; /* The name of the table */
    int Offset;      /* The offset of the file position from 0 */
};
#pragma pack(0)

#pragma pack(1)
struct Offset
{
    struct TableOffset **Offsets; /** Offsets to tables */
    int ImwebOffset;              /** Offset to the immediate-write-buffer */
};
#pragma pack(0)

#pragma pack(1)
struct FileMetadata
{
    struct Offset *Offset; /* Offset values*/
    time_t CreatedAt;      /* When was this database created */
    time_t LastModified;   /* When was this last modified */
    int TableCount;        /* Number of tables in database */
};
#pragma pack()

struct TableOffset *NewTableOffset(const char *tableName, int offset);
struct Offset *NewOffset(struct TableOffset **offsets, int imwebOffset);
struct FileMetadata *NewFileMetadata(struct Offset *offset, int tableCount);
char *FormatTableOffset(const struct TableOffset const *tableOffset);
void IntrospectMetadata(const struct FileMetadata *metadata);
void WriteMetadataToFile(struct FileMetadata *metadata, FILE file);
void ReadMetadataToFile(struct FileMetadata *metadata, FILE file);

int main()
{
    struct TableOffset *animalsOffset = NewTableOffset("animals", 0);
    struct TableOffset *carsOffset = NewTableOffset("cars", 1);
    if (animalsOffset == NULL || carsOffset == NULL)
    {
        fprintf(stderr, "Could not create a table offset");
        return -1;
    }

    struct TableOffset *allOffsets[] = {animalsOffset, carsOffset};
    struct Offset *offset = NewOffset(allOffsets, 1);
    if (offset == NULL)
    {
        fprintf(stderr, "Could not create an offset");
        return -1;
    }

    struct FileMetadata *metadata = NewFileMetadata(offset, 2);
    if (metadata == NULL)
    {
        fprintf(stderr, "Could not create metadata");
        return -1;
    }

    IntrospectMetadata(metadata);

    free(animalsOffset);
    free(carsOffset);
    free(offset);
    free(metadata);
}

struct TableOffset *NewTableOffset(const char *tableName, int offset)
{
    struct TableOffset *tableOffset;
    tableOffset = malloc(sizeof(struct TableOffset));

    if (tableOffset == NULL)
    {
        return NULL;
    }

    tableOffset->Offset = offset;
    tableOffset->TableName = malloc(strlen(tableName) + 1);
    if (tableOffset->TableName == NULL)
    {
        free(tableOffset);
        return NULL;
    }
    strcpy(tableOffset->TableName, tableName);

    return tableOffset;
}

char *FormatTableOffset(const struct TableOffset const *tableOffset)
{
    size_t wBytes = sizeof(char *) * OFFSET_TXT_WITH_PADDING + strlen(tableOffset->TableName) + sizeof(int);
    char *str = malloc(wBytes);
    if (str == NULL)
    {
        return NULL;
    }
    sprintf(str, "<TableOffset> Tablename: %s, Offset: %d\n", tableOffset->TableName, tableOffset->Offset);
    return str;
}

struct Offset *NewOffset(struct TableOffset **offsets, int imwebOffset)
{
    struct Offset *offset;
    offset = malloc(sizeof(struct Offset));

    if (offset == NULL)
    {
        return NULL;
    }

    offset->Offsets = offsets;
    offset->ImwebOffset = imwebOffset;

    return offset;
}

struct FileMetadata *NewFileMetadata(struct Offset *offset, int tableCount)
{
    struct FileMetadata *fMetadata;
    time_t now;

    fMetadata = malloc(sizeof(struct FileMetadata));
    if (fMetadata == NULL)
    {
        return NULL;
    }

    fMetadata->Offset = offset;

    if (!time(&now))
    {
        free(fMetadata);
        return NULL;
    }

    fMetadata->CreatedAt = now;
    fMetadata->LastModified = now;
    fMetadata->TableCount = tableCount;

    return fMetadata;
}

void IntrospectMetadata(const struct FileMetadata *metadata)
{
    int count = 0;
    struct TableOffset *currOffset = *(metadata->Offset->Offsets);

    if (currOffset != NULL)
    {
        for (; count < metadata->TableCount; count++)
        {
            fprintf(stdout, "<FileMetadata Introspection> %s", FormatTableOffset(*(metadata->Offset->Offsets++)));
        }
    }
    fprintf(stdout, "<FileMetadata Introspection> Immediate-Write-Buffer Offset: %d\n", metadata->Offset->ImwebOffset);
    fprintf(
        stdout,
        "<FileMetadata Introspection> CreatedAt: %ld, LastModified: %ld, TableCount: %d\n",
        (long)metadata->CreatedAt,
        (long)metadata->LastModified,
        metadata->TableCount);
}

void WriteMetadataToFile(struct FileMetadata *metadata, FILE file) {}
void ReadMetadataToFile(struct FileMetadata *metadata, FILE file) {}
