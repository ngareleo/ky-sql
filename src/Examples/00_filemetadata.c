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
    writableNoop = fopen(NOOP_FILE, "w");

    do
    {
        if (!WriteMetadataToFile(writableNoop, meta, CreateWritableFromMetadata) == 0)
        {
            fprintf(stderr, "<main> ERR! Couldn't write metadata to file :( \n");
            fclose(writableNoop);
            break;
        }

        fclose(writableNoop);

        readableNoop = fopen(NOOP_FILE, "r");
        if (!readableNoop)
        {
            perror("fopen");
            fprintf(stderr, "<main> ERR! Couldn't open file :( \n");
            break;
        }

        if (ReadMetadataFromFile(readableNoop, &metaFromFile, CreateMetadataFromWritable) == 0)
        {
            fprintf(stdout, "<main> FileMetadata read from file success \n");
            IntrospectMetadata(metaFromFile);
        }

        fclose(readableNoop);
    } while (0);

    FreeFileMetadata(meta);
    FreeFileMetadata(metaFromFile);
    FreeSchemaDefinition(bookstoreSchema);
}