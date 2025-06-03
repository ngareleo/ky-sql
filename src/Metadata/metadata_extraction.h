#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "metadata_offsets.h"
#include "metadata_schema.h"

#define OFFSET_TXT_WITH_PADDING 60
#define MAX_TABLENAME_LENGTH 100
#define MAX_TABLE_COUNT 100
#define MAX_PROPERTY_NAME_SIZE 100

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
    struct PersistedSchema
    {
        struct PersistedTableDefinition
        {
            struct PersistedTableColDefinition
            {
                char ColumnName[MAX_PROPERTY_NAME_SIZE];
                enum SchemaType ColumnType;
                bool ColumnIsUnique;
                char ColumnDefaultValue[MAX_PROPERTY_NAME_SIZE]; /** We will perform casting according to the type */
                time_t ColumnCreatedAt;
                time_t ColumnLastModified;
            } ColumnDefs[MAX_TABLE_COLUMN_COUNT];
            char TableName[MAX_PROPERTY_NAME_SIZE];
            time_t CreatedAt;
            time_t LastModified;
        } TableDefs[MAX_TABLE_COUNT];
        char TagName[MAX_PROPERTY_NAME_SIZE];
        time_t CreatedAt;
        time_t LastModified;
    } Schema;
    // Offsets
    struct PersistedTableOffset
    {
        char TableName[MAX_PROPERTY_NAME_SIZE]; /**  The name of the table */
        int Offset;                             /** The offset of the file position from 0 */
    } Offsets[MAX_TABLE_COUNT];                 /** Offsets to tables */
    int ImwebOffset;                            /** Offset values*/

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