#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../Metadata/metadata.h"
#include "../Mocks/mocks.h"

const char *NOOP_FILE = "noop.kysql";

int main()
{
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

    return 0;
}