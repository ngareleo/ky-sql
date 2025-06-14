#ifndef METADATA_EXTRACTION_H
#define METADATA_EXTRACTION_H

#include <time.h>
#include "metadata_offsets.h"
#include "metadata_schema.h"

#define OFFSET_TXT_WITH_PADDING 60
#define MAX_TABLENAME_LENGTH 100
#define MAX_TABLE_COUNT 100
#define MAX_PROPERTY_NAME_SIZE 100

typedef struct
{
    SchemaDefinition *Schema; /** A repr of the SQL schema */
    Offset *Offset;           /** Offset values*/
    time_t CreatedAt;         /** When was this database created */
    time_t LastModified;      /** When was this last modified */
    int TableCount;           /** Number of tables in database */
} FileMetadata;

#pragma pack(1)
typedef struct
{
    struct PersistedTableOffset
    {
        char TableName[MAX_PROPERTY_NAME_SIZE];                  /**  The name of the table */
        int Offset;                                              /** The offset of the file position from 0 */
    } Offsets[MAX_TABLE_COUNT];                                  /** Offsets to tables */
    struct PersistedSchema                                       /** */
    {                                                            /** */
        struct PersistedTableDefinition                          /** */
        {                                                        /** */
            struct PersistedTableColDefinition                   /** */
            {                                                    /** */
                char ColumnName[MAX_PROPERTY_NAME_SIZE];         /** The name of the column */
                enum SchemaType ColumnType;                      /** The data type of the column */
                bool ColumnIsUnique;                             /** Is this column unique*/
                char ColumnDefaultValue[MAX_PROPERTY_NAME_SIZE]; /** We will perform casting according to the type */
                time_t ColumnCreatedAt;                          /** When was the column created */
                time_t ColumnLastModified;                       /** When the column was last modified */
            } ColumnDefs[MAX_TABLE_COLUMN_COUNT];                /** Columns in the table */
            time_t CreatedAt;                                    /** When was this table created */
            time_t LastModified;                                 /** When was this table last modified */
            int ColumnCount;                                     /** The number of columns */
            char TableName[MAX_PROPERTY_NAME_SIZE];              /** Name of the table */
        } TableDefs[MAX_TABLE_COUNT];                            /**Tables */
        char TagName[MAX_PROPERTY_NAME_SIZE];                    /** Unique name for schema  */
        time_t CreatedAt;                                        /** When was this schema created. Usually when the db file was initialized */
        time_t LastModified;                                     /** When was this schema last updated */
    } Schema;                                                    /** The database schema */
    int OffsetCount;                                             /** Number of offsets */
    time_t CreatedAt;                                            /** When was this database created */
    time_t LastModified;                                         /** When was this last modified */
    int ImwebOffset;                                             /** Offset values*/
    int TableCount;                                              /** Number of tables in database */
} PersistedFileMetadata;                                        
#pragma pack(0)

FileMetadata *NewFileMetadata(Offset *, SchemaDefinition *schema);
void FreeFileMetadata(FileMetadata *);
char *FormatTableOffset(const TableOffset *);
void IntrospectMetadata(const FileMetadata *);
/**
 * Writes a FileMetadata struct into a 'file' in-form of a PersistedFileMetadata
 * Also takes a mapping function that constructs the PersistedFileMetadata struct.
 * Does the required stack allocation.
 */
int WriteMetadataToFile(FILE *, FileMetadata *, PersistedFileMetadata *(const FileMetadata *));
/**
 * Reads out a FileMetadata struct from the 'file' into 'in'.
 * Also takes a mapping function that constructs the output struct.
 */
int ReadMetadataFromFile(FILE *, FileMetadata **, FileMetadata *(const PersistedFileMetadata *));
FileMetadata *BootFileMetadataFromFile(const PersistedFileMetadata *);

#endif