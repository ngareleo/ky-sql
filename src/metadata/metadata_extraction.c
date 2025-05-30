#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#pragma pack(1)
struct TableOffset
{
    char *TableName; /* The name of the table */
    int Offset;      /* The offset of the file position from 0 */
};
#pragma pack(0)

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

const char *FormatTableOffset() {}

#pragma pack(1)
struct Offset
{
    struct TableOffset *Offsets; /** Offsets to tables */
    int ImwebOffset;             /** Offset to the immediate-write-buffer */
};
#pragma pack(0)

struct Offset *NewOffset(struct TableOffset *offsets, int imwebOffset)
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

#pragma pack(1)
struct FileMetadata
{
    struct Offset *Offset; /* Offset values*/
    time_t CreatedAt;      /* When was this database created */
    time_t LastModified;   /* When was this last modified */
};
#pragma pack()

struct FileMetadata *NewFileMetadata(struct Offset *offset)
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

    return fMetadata;
}

void IntrospectMetadata(struct FileMetadata *metadata)
{
    fprintf(
        stdout,
        "FileMetadata Introspection: createdAt %ld, lastModified %ld\n",
        (long)metadata->CreatedAt,
        (long)metadata->LastModified);
}

void WriteMetadataToFile(struct FileMetadata *metadata, FILE file) {}
void ReadMetadataToFile(struct FileMetadata *metadata, FILE file) {}

int main()
{
    struct TableOffset *sampleOffset = NewTableOffset("animals", 0);
    if (sampleOffset == NULL)
    {
        fprintf(stderr, "Could not create a table offset");
        return -1;
    }

    struct Offset *offset = NewOffset(sampleOffset, 1);
    if (offset == NULL)
    {
        fprintf(stderr, "Could not create an offset");
        return -1;
    }

    struct FileMetadata *metadata = NewFileMetadata(offset);
    if (metadata == NULL)
    {
        fprintf(stderr, "Could not create metadata");
        return -1;
    }

    IntrospectMetadata(metadata);

    free(sampleOffset);
    free(offset);
    free(metadata);
}