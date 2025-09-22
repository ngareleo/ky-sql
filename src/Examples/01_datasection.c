#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../Metadata/metadata.h"
#include "../Mocks/mocks.h"

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

int main()
{
    char *file = "noop.kysql";
    FileMetadata *meta;
    FILE *readable = fopen(file, "r");
}