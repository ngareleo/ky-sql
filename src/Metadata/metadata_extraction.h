#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "metadata_offsets.h"

#define OFFSET_TXT_WITH_PADDING 60
#define MAX_TABLENAME_LENGTH 100
#define MAX_TABLE_COUNT 100

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