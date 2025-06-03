#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metadata_extraction.h"
#include "metadata_offsets.h"
#include "metadata_schema.h"

const struct Offset *CreateMocKOffset()
{
    struct TableOffset *animalsOffset, *carsOffset;
    if ((animalsOffset = NewTableOffset("animals", 0)) == NULL ||
        (carsOffset = NewTableOffset("cars", 1)) == NULL)
    {
        fprintf(stderr, "<main> Could not create a table offset");
        return NULL;
    }

    struct TableOffset **all = malloc(sizeof(struct TableOffset *) * 2);
    if (!all)
    {
        return NULL;
    }

    all[0] = animalsOffset;
    all[1] = carsOffset;
    struct Offset *offset = NewOffset(all, 2);
    return offset;
}

const struct SchemaDefinition *CreateMockSchema(time_t *duration)
{
    struct TableColDefinition *idCol, *nameCol, *durationCol, *relaseDateCol;
    struct TableDefinition *songTableDef;
    struct SchemaDefinition *music;

    music = ((idCol = NewTableColumn("Id", ID, true, NULL)) == NULL ||
             (nameCol = NewTableColumn("Name", STRING, false, NULL)) == NULL ||
             (durationCol = NewTableColumn("Duration", FLOAT, false, duration)) == NULL ||
             (relaseDateCol = NewTableColumn("Release Date", DATE, false, NULL)) == NULL ||
             (songTableDef = NewTableDefinition("Songs", idCol, nameCol, durationCol, relaseDateCol)) == NULL)
                ? NULL
                : NewSchemaDefinition(songTableDef);

    if (!music)
    {
        if (songTableDef)
        {
            FreeTableDefinition(songTableDef);
        }
        else
        {
            FreeTableColDefinition(idCol);
            FreeTableColDefinition(durationCol);
            FreeTableColDefinition(relaseDateCol);
            FreeTableColDefinition(nameCol);
        }
    }

    return music;
}

const char *NOOP_FILE = "noop.kysql";

int main()
{

    time_t duration = 1000;
    const struct SchemaDefinition *music = CreateMockSchema(&duration);
    FreeSchemaDefinition(music);

    struct Offset *offset = CreateMocKOffset();
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