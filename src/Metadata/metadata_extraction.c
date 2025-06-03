#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metadata_extraction.h"
#include "metadata_offsets.h"

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
            return NULL;
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
