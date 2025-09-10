#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../Metadata/metadata.h"
#include "../Mocks/mocks.h"

typedef struct
{
    /** A reliable downstream value */
    int rows;

    /** The table being targeted by insertion */
    int tableKey;

    /** Points to the character data*/
    char **rows;
} DataSectionWriteRequest;

typedef struct
{
} DataSectionReadRequest;

int main()
{
    char *file = "noop.kysql"; // Database target
    FileMetadata *meta;
    FILE *readable = fopen(file, "r");

    // !! OrderedPayload sucks. Look for better name
    DataSectionWriteRequest *payload, *readFromFile;

    do
    {
        payload = GetBookStoreSchemaData("");
        if (!payload)
        {
            break;
        }

        if (InitializeMetadata(readable, meta) != 0)
        {
            break;
        }

        if (WriteToDataSection(meta))
        {
            break;
        }

        if (ReadFromDataSection(readFromFile))
        {
            break;
        }

    } while (0);

    fclose(readable);
    FreeFileMetadata(meta);
    FreeDataSectionWriteRequest(readable);
    FreeDataSectionWriteRequest(readFromFile);
}

int InitializeMetadata()
{
    time_t duration = 1000;
    FILE *writableNoop, *readableNoop;
    SchemaDefinition *bookstoreSchema;
    FileMetadata *constructedMeta, *metaFromFile;

    bookstoreSchema = CreateBookStoreSchemaDefinition();
    if (!bookstoreSchema)
    {
        fprintf(stderr, "(main-err) Could not create book store schema\n");
        return -1;
    }

    constructedMeta = CreateNewFileMetadataFromSchema(bookstoreSchema);
    if (!constructedMeta)
    {
        FreeSchemaDefinition(bookstoreSchema);
        fprintf(stderr, "(main-err) Could not create file metadata\n");
        return -1;
    }

    fprintf(stdout, "(main) Metadata created successfully\n");
    IntrospectMetadata(constructedMeta);

    do
    {
        // !!
        // !! Only break out of this loop, avoid returning to allow cleanups

        writableNoop = fopen(NOOP_FILE, "w");
        if (!writableNoop)
        {
            fprintf(stderr, "(00-file-metadata-main-err) Couldn't open stream to file --> %s :( \n", NOOP_FILE);
            break;
        }

        if (WriteMetadataToFile(writableNoop, constructedMeta, CreateWritableFromMetadata) != 0)
        {
            fprintf(stderr, "(00-file-metadata-main-err) Couldn't write metadata to file \n");
            break;
        }

        fprintf(stdout, "(00-file-metadata-main-info) Metadata written to file \n");

        fflush(writableNoop);
        readableNoop = fopen(NOOP_FILE, "r");
        if (!readableNoop)
        {
            // !! Shouldn't fail twice
            assert(readableNoop != NULL);
            break;
        }

        if (ReadMetadataFromFile(readableNoop, &metaFromFile, CreateMetadataFromWritable) != 0)
        {
            fprintf(stderr, "(00-file-metadata-main-err) Couldn't load metadata from file \n");
            break;
        }

        fprintf(stdout, "(00-file-metadata-main-info) file metadata read from file successfully :) \n");
        IntrospectMetadata(metaFromFile);
    } while (0);

    fclose(writableNoop);
    fclose(readableNoop);
    FreeFileMetadata(constructedMeta);
    FreeFileMetadata(metaFromFile);
    FreeSchemaDefinition(bookstoreSchema);
}