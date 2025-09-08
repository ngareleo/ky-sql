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
    time_t duration = 1000;
    FILE *writableNoop, *readableNoop;
    SchemaDefinition *bookstoreSchema;
    FileMetadata *meta, *metaFromFile;

    bookstoreSchema = CreateBookStoreSchemaDefinition();
    if (!bookstoreSchema)
    {
        fprintf(stderr, "(main-err) Could not create book store schema\n");
        return -1;
    }

    meta = CreateNewFileMetadataFromSchema(bookstoreSchema);
    if (!meta)
    {
        FreeSchemaDefinition(bookstoreSchema);
        fprintf(stderr, "(main-err) Could not create file metadata\n");
        return -1;
    }

    fprintf(stdout, "(main) Metadata created successfully\n");
    IntrospectMetadata(meta);

    do
    {
        writableNoop = fopen(NOOP_FILE, "w");
        if (!writableNoop)
        {
            fprintf(stderr, "(00-file-metadata-main-err) Couldn't open stream to file --> %s :( \n", NOOP_FILE);
            break;
        }

        if (WriteMetadataToFile(writableNoop, meta, CreateWritableFromMetadata) != 0)
        {
            fprintf(stderr, "(00-file-metadata-main-err) Couldn't write metadata to file \n");
            fclose(writableNoop);
            break;
        }
        else
        {
            fclose(writableNoop);
            fprintf(stdout, "(00-file-metadata-main-info) Metadata written to file \n");
        }

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
            fclose(writableNoop);
            break;
        }
        else
        {
            fprintf(stdout, "(00-file-metadata-main-info) FileMetadata read from file success \n");
        }

        IntrospectMetadata(metaFromFile);
        fclose(readableNoop);
    } while (0);

    FreeFileMetadata(meta);
    FreeFileMetadata(metaFromFile);
    FreeSchemaDefinition(bookstoreSchema);
}