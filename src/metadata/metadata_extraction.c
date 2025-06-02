#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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
    int ImwebOffset;              /** Offset to the immediate-write-buffer */
};

struct FileMetadata
{
    struct Offset *Offset; /** Offset values*/
    time_t CreatedAt;      /** When was this database created */
    time_t LastModified;   /** When was this last modified */
    int TableCount;        /** Number of tables in database */
};

#pragma pack(1)
struct PersistedFileMetadata
{
    // Offsets
    struct PersistedTableOffset
    {
        char TableName[1000];   /**  The name of the table */
        int Offset;             /** The offset of the file position from 0 */
    } Offsets[MAX_TABLE_COUNT]; /** Offsets to tables */
    int ImwebOffset;            /** Offset values*/

    // Table Information
    int TableCount; /** Number of tables in database */

    // File Metadata
    time_t CreatedAt;    /** When was this database created */
    time_t LastModified; /** When was this last modified */
};
#pragma pack(0)

struct TableOffset *NewTableOffset(const char *, int);
struct Offset *NewOffset(struct TableOffset **, int);
void FreeOffset(struct Offset *, int);
struct FileMetadata *NewFileMetadata(struct Offset *, int);
void FreeFileMetadata(struct FileMetadata *);
char *FormatTableOffset(const struct TableOffset *);
void IntrospectMetadata(const struct FileMetadata *);
/**
 * Writes a FileMetadata struct into a 'file' in-form of a PersistedFileMetadata
 * Also takes a mapping function that constructs the PersistedFileMetadata struct.
 * Does the required stack allocation.
 */
int WriteMetadataToFile(FILE *, struct FileMetadata *, struct PersistedFileMetadata *(const struct FileMetadata *));
/**
 * Reads out a FileMetadata struct from the 'file' into 'in'.
 * Also takes a mapping function that constructs the output struct.
 */
int ReadMetadataFromFile(FILE *, struct FileMetadata **, struct FileMetadata *(const struct PersistedFileMetadata *));
struct FileMetadata *BootFileMetadataFromFile(const struct PersistedFileMetadata *);
struct PersistedFileMetadata *LoadFileMetadataIntoFile(const struct FileMetadata *);

const char *NOOP_FILE = "noop.kysql";

int main()
{
    struct TableOffset *animalsOffset = NewTableOffset("animals", 0);
    struct TableOffset *carsOffset = NewTableOffset("cars", 1);
    if (animalsOffset == NULL || carsOffset == NULL)
    {
        fprintf(stderr, "<main> Could not create a table offset");
        return -1;
    }

    struct TableOffset *allOffsets[] = {animalsOffset, carsOffset};
    struct Offset *offset = NewOffset(allOffsets, 1);
    if (offset == NULL)
    {
        fprintf(stderr, "<main> Could not create an offset");
        return -1;
    }

    struct FileMetadata *metadata = NewFileMetadata(offset, 2);
    if (metadata == NULL)
    {
        fprintf(stderr, "<main> Could not create metadata");
        return -1;
    }

    IntrospectMetadata(metadata);

    struct FileMetadata *metaFromFile = NULL;
    FILE *writableNoop, *readableNoop;
    writableNoop = fopen(NOOP_FILE, "w");

    do
    {
        if (!WriteMetadataToFile(writableNoop, metadata, LoadFileMetadataIntoFile) == 0)
        {
            fclose(writableNoop);
            break;
        }

        // Close it before we open readable stream on the same file
        fclose(writableNoop);

        readableNoop = fopen(NOOP_FILE, "r");

        if (!readableNoop)
        {
            perror("fopen");
            fprintf(stderr, "<main> ERR! Couldn't open file :( \n");
            break;
        }

        if (ReadMetadataFromFile(readableNoop, &metaFromFile, BootFileMetadataFromFile) == 0)
        {
            fprintf(stdout, "<main> FileMetadata read from file success \n");
            IntrospectMetadata(metaFromFile);
        }

        fclose(readableNoop);

    } while (0);

    FreeFileMetadata(metadata);
    FreeFileMetadata(metaFromFile);
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

void FreeTableOffset(struct TableOffset *offset)
{
    if (!offset)
    {
        return;
    }

    free(offset->TableName);
    free(offset);
}

const size_t wBytes = 1000;

char *FormatTableOffset(const struct TableOffset const *tableOffset)
{
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

void FreeOffset(struct Offset *offset, int n)
{

    if (offset == NULL)
    {
        return;
    }

    for (int c = 0; c < n; c++)
    {
        FreeTableOffset(offset->Offsets[c]);
    }

    free(offset);
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

void FreeFileMetadata(struct FileMetadata *metadata)
{
    if (metadata == NULL)
    {
        return;
    }

    FreeOffset(metadata->Offset, metadata->TableCount);
    free(metadata);
}

void IntrospectMetadata(const struct FileMetadata *metadata)
{

    if (metadata == NULL)
    {
        fprintf(stderr, "Cannot introspect null value\n");
        return;
    }

    struct TableOffset *currOffset = *(metadata->Offset->Offsets);

    fprintf(stdout, "<FileMetadata Introspection> Immediate-Write-Buffer Offset: %d\n", metadata->Offset->ImwebOffset);
    fprintf(
        stdout,
        "<FileMetadata Introspection> CreatedAt: %ld, LastModified: %ld, TableCount: %d\n",
        (long)metadata->CreatedAt,
        (long)metadata->LastModified,
        metadata->TableCount);

    if (currOffset != NULL)
    {
        for (int count = 0; count < metadata->TableCount; count++)
        {
            fprintf(stdout, "<FileMetadata Introspection> %s", FormatTableOffset(*(metadata->Offset->Offsets + count)));
        }
    }
}

int WriteMetadataToFile(FILE *file, struct FileMetadata *in, struct PersistedFileMetadata *map(const struct FileMetadata *metadata))
{
    int status = -1;
    struct PersistedFileMetadata *target;

    do
    {
        if (map == NULL || (target = map(in)) == NULL)
        {
            break;
        }

        if (!fwrite(target, sizeof(struct PersistedFileMetadata), 1, file))
        {
            fprintf(stderr, "ERR! <WriteMetadataToFile> Could not write metadata to file \n");
            perror("fwrite");
            break;
        }

        fprintf(stdout, "<WriteMetadataToFile> Metadata written into file\n");
        status = 0;
    } while (0);

    if (target != NULL)
    {
        free(target);
    }
    return status;
}

int ReadMetadataFromFile(FILE *file, struct FileMetadata **out, struct FileMetadata *map(const struct PersistedFileMetadata *metadata))
{
    struct PersistedFileMetadata *buffer;
    int status = -1;

    do
    {
        if ((buffer = malloc(sizeof(struct PersistedFileMetadata))) == NULL)
        {
            break;
        }

        if (fread(buffer, sizeof(struct PersistedFileMetadata), 1, file) == 0)
        {
            fprintf(stderr, "ERR! <ReadMetadataFromFile> Could not read metadata from file \n");
            perror("fread");
            break;
        }

        if (map == NULL || (*out = map(buffer)) == NULL)
        {
            break;
        }

        status = 0;
    } while (0);

    if (buffer != NULL)
    {
        free(buffer);
    }
    return status;
}

struct FileMetadata *BootFileMetadataFromFile(const struct PersistedFileMetadata *metadata)
{
    if (metadata == NULL)
    {
        return NULL;
    }

    struct FileMetadata *target;
    if ((target = malloc(sizeof(struct FileMetadata))) == NULL)
    {
        return NULL;
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->TableCount;

    target->Offset = malloc(sizeof(struct Offset));
    if (!target->Offset)
    {
        free(target);
        return NULL;
    }

    target->Offset->Offsets = malloc(sizeof(struct TableOffset *) * metadata->TableCount);
    if (!target->Offset->Offsets)
    {
        free(target->Offset);
        free(target);
        return NULL;
    }

    for (int count = 0; count < metadata->TableCount; count++)
    {
        target->Offset->Offsets[count] = NewTableOffset(metadata->Offsets[count].TableName, metadata->Offsets[count].Offset);
        if (!target->Offset->Offsets[count])
        {
            FreeOffset(target->Offset, count);
            free(target->Offset);
            free(target);
        }
    }

    IntrospectMetadata(target);
    return target;
}

struct PersistedFileMetadata *LoadFileMetadataIntoFile(const struct FileMetadata *metadata)
{
    if (metadata == NULL)
    {
        return NULL;
    }

    struct PersistedFileMetadata *target;
    if ((target = malloc(sizeof(struct PersistedFileMetadata))) == NULL)
    {
        return NULL;
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->TableCount;
    target->ImwebOffset = metadata->Offset->ImwebOffset;

    for (int count = 0; count < metadata->TableCount; count++)
    {
        target->Offsets[count].Offset = metadata->Offset->Offsets[count]->Offset;
        strcpy(target->Offsets[count].TableName, metadata->Offset->Offsets[count]->TableName);
    }

    return target;
}