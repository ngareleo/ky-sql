/**
 * This example is one of the larger sections
 *
 * We go from [ordered data --> writing all data to a file --> Read all data from file --> Display data]
 * Later we will get data via SQL statements. That will be handled by the Language Layer
 *
 * The Language layer will receive some statement
 * From the statement, it can identify the table from the schema
 *
 * The data is extracted and the output will be a character array that is separated by null values.
 * Another good thing is that we have a schema, so reading will be error proof.
 *
 * !! This format is tricky to work with in code (around queries) so we'll need somthing easier
 * !! Won't think too much about this until we feel the pain
 *
 * In this example, we'll write the data directly to the data section.
 *
 * The Writer is responsible of this. It takes a few pieces of information.
 * - Need to know where to write.
 * - What to write
 *
 * Before going any further, one of the coming sections is the immediate write buffer
 * It will be a writing strategy used by the writer to improve write times since writes are blocking in the forseeable future to make things simple.
 * Data is written at the end of file for instant writes. Data will be later be moved to the data section.
 *
 * !! This section will deal with writes to the data section and retreival.
 * We'll have to read the data
 * The Language Layer again comes to play. It translates SQL query texts to:
 * - Which table to read from (Table ID)
 * - Which columns to read (Column Selection)
 * - Where to find data (Search)
 * - How much data to read (Sizing)
 * - How to present the data
 *      - Ordering
 *      - Filtering
 *
 * Todo: We will have to come up with data structures the Language layer can use to relay this information to other layers
 * For now, we're interested in (1). Others will be covered in later examples.
 *
 * The reader is responsible for doing a direct read to file.
 * It needs the following information:
 * - Where do I read
 * - How much do I read
 * * Later we will look at transactions.
 *
 * In both writing and reading scenarios,
 * !! We need to map a request from Language Layer to structure that the Reader and Writer will accept.
 * !! I'll call this layer the Translation layer.
 *
 * One more thing, both Reader and Writer pick up requests from a common queue. They don't decide when.
 * They pull data from a queue and push results out to a stack. When the queue is empty they sleep.
 * Other layers can access the queue.
 *
 * The translation layer will
 *  - Translate a table write/read to a file location.
 *  - Translate a column write/read to a file locaton.
 *  - Translate a search to an array of file locations to read from.
 *  - Translate size to read control metadata.
 *
 *
 * !! In this example, we will build both the Reader, Writer, Transalation Layer.
 * !! Then we'll use harcoded mock values of the Language Layer.
 * !! For Transaltion layer, we'll implement function (1)
 *
 */

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
    DataSectionWriteRequest *writeReq;
    DataSectionReadRequest *readReq;

    do
    {
        writeReq = GetBookStoreSchemaData("");
        if (!writeReq)
        {
            break;
        }

        if (InitializeMetadata(readable, meta) != 0)
        {
            break;
        }

        if (WriteToDataSection(writeReq, meta))
        {
            break;
        }

        if (ReadFromDataSection(readReq, meta))
        {
            break;
        }

    } while (0);

    fclose(readable);
    FreeFileMetadata(meta);
    FreeDataSectionWriteRequest(writeReq);
    FreeDataSectionReadRequest(readReq);
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