#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metadata_extraction.h"
#include "metadata_offsets.h"

struct FileMetadata *BootFileMetadataFromFile(const struct PersistedFileMetadata *metadata)
{
    struct FileMetadata *target;
    struct TableOffset **tempTbOff;

    if (!metadata)
    {
        return NULL;
    }

    tempTbOff = malloc(sizeof(struct TableOffset *) * metadata->OffsetCount);
    if (!tempTbOff)
    {
        fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. malloc failed. \n");
        return NULL;
    }

    for (int c = 0; c < metadata->OffsetCount; c++)
    {
        tempTbOff[c] = NewTableOffset(metadata->Offsets[c].TableName, metadata->Offsets[c].Offset);
        if (!tempTbOff[c])
        {
            for (int c2 = 0; c2 < c; c2++)
            {
                free(tempTbOff[c2]);
            }
            free(tempTbOff);
            fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. couldn't create table offset. \n");
            return NULL;
        }
    }

    target = malloc(sizeof(struct FileMetadata));
    if (!target)
    {
        fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. malloc failed. \n");
        free(tempTbOff);
        return NULL;
    }

    target->Offset = malloc(sizeof(struct Offset));
    if (!target->Offset)
    {
        fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. malloc failed. \n");
        free(tempTbOff);
        free(target);
        return NULL;
    }

    // No need for a check. Guranteed tempTbOff is const
    target->Offset->Offsets = tempTbOff;

    target->Offset->ImwebOffset = metadata->ImwebOffset;
    target->Offset->OffsetCount = metadata->OffsetCount;
    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->TableCount;

    IntrospectMetadata(target);
    return target;
}

struct PersistedFileMetadata *MapToPersistedMetadata(const struct FileMetadata *metadata)
{
    struct PersistedFileMetadata *target;

    if (!metadata)
    {
        fprintf(stderr, "[MapToPersistedMetadata] Mapping to persisted metadata failed. malloc failed. \n");
        return NULL;
    }

    target = malloc(sizeof(struct PersistedFileMetadata));
    if (!target)
    {
        fprintf(stderr, "[MapToPersistedMetadata] Mapping to persisted metadata failed. malloc failed. \n");
        return NULL;
    }

    for (int count = 0; count < metadata->TableCount; count++)
    {
        target->Offsets[count].Offset = metadata->Offset->Offsets[count]->Offset;
        strcpy(target->Offsets[count].TableName, metadata->Offset->Offsets[count]->TableName);
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->TableCount;
    target->ImwebOffset = metadata->Offset->ImwebOffset;

    return target;
}

struct FileMetadata *NewFileMetadata(struct Offset *offset, struct SchemaDefinition *schema)
{
    struct FileMetadata *fMetadata;
    time_t now;

    fMetadata = malloc(sizeof(struct FileMetadata));
    if (fMetadata == NULL)
    {
        fprintf(stderr, "[NewFileMetadata] cannot create metadata. malloc failed. \n");
        return NULL;
    }

    if (!time(&now))
    {
        fprintf(stderr, "[NewFileMetadata] cannot create metadata. malloc failed. \n");
        free(fMetadata);
        return NULL;
    }

    fMetadata->Offset = offset;
    fMetadata->CreatedAt = now;
    fMetadata->LastModified = now;

    return fMetadata;
}

void FreeFileMetadata(struct FileMetadata *metadata)
{
    if (metadata == NULL)
    {
        return;
    }

    FreeOffset(metadata->Offset);
    free(metadata);
}

char *IntrospectMetadata(const struct FileMetadata *metadata)
{
    FILE *memstream;
    char *buffer;
    size_t size;

    if (!metadata)
    {
        fprintf(stderr, "[IntrospectMetadata] Couldn't introspect metadata. metadata is null.\n");
        return NULL;
    }

    memstream = open_memstream(&buffer, &size);
    if (!memstream)
    {
        fprintf(stderr, "[IntrospectMetadata] Couldn't introspect metadata. metadata is null. open_memstream failed.\n");
        return NULL;
    }

    fprintf(memstream, "[IntrospectMetadata] ############# Start of log #############\n\n");

    fprintf(memstream, "immediate-write-buffer-offset = %s\n", metadata->Offset->ImwebOffset);
    fprintf(memstream, "created-at                    = %s\n", metadata->CreatedAt);
    fprintf(memstream, "last-modified                 = %s\n", metadata->LastModified);
    fprintf(memstream, "table-count                   = %d\n", metadata->TableCount);
    fprintf(memstream, "offsets                       = \n");
    for (int c = 0; c < metadata->TableCount; c++)
    {
        char *formatted = FormatTableOffset(metadata->Offset->Offsets[c]);
        if (formatted)
        {
            fprintf(memstream, "\t%s\n", formatted);
        }
    }
    fprintf(memstream, "[IntrospectMetadata] ############# End of log #############\n\n");
    fclose(memstream);
    return buffer;
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
            fprintf(stderr, "[WriteMetadataToFile] Could not write metadata to file. fwrite failed \n");
            break;
        }
        status = 0;
    } while (0);

    free(target);
    return status;
}

int ReadMetadataFromFile(FILE *file, struct FileMetadata **out, struct FileMetadata *map(const struct PersistedFileMetadata *metadata))
{
    struct PersistedFileMetadata *buffer;
    int status = -1;
    if (map == NULL)
    {
        fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. map is null \n");
    }

    do
    {
        buffer = malloc(sizeof(struct PersistedFileMetadata));
        if (!buffer)
        {
            fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. malloc failed \n");
            break;
        }

        fread(buffer, sizeof(struct PersistedFileMetadata), 1, file);
        if (!buffer)
        {
            fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. fread failed \n");
            break;
        }

        *out = map(buffer);
        if (!out)
        {
            fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. malloc failed \n");
        }

        status = 0;
    } while (0);

    free(buffer);
    return status;
}
