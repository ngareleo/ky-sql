#ifndef METADATA_EXTRACTION_H
#define METADATA_EXTRACTION_H

#include <time.h>
#include "metadata_offsets.h"
#include "metadata_schema.h"
#include "metadata_storage.h"

#define OFFSET_TXT_WITH_PADDING 60
#define MAX_TABLENAME_LENGTH 100
#define MAX_TABLE_COUNT 100
#define MAX_PROPERTY_NAME_SIZE 100

typedef struct
{
    SchemaDefinition *Schema; /** A repr of the SQL schema */
    StorageMeta *Storage;     /** Storage information like the sizes of tables */
    Offset *Offset;           /** Offset values*/
    time_t CreatedAt;         /** When was this database created */
    time_t LastModified;      /** When was this last modified */
} FileMetadata;

#pragma pack(1)
typedef struct
{
    struct WritableTableOffset                                   /** */
    {                                                            /** */
        int TableOffsetId;                                       /** The name of the table */
        int TableOffset;                                         /** The offset of the file position from 0 */
    } TableOffsets[MAX_TABLE_COUNT];                             /** Offsets to tables */
    struct WritableStorageMeta                                   /** */
    {                                                            /** */
        struct WritableColumnStorage                             /** */
        {                                                        /** */
            int Id;                                              /**  The id of the column */
            int Padding;                                         /**  Byte distance from start of the row */
            int SequencePos;                                     /**  The position in the schema */
        } ColInfo[MAX_TABLE_COLUMN_COUNT];                       /**  Information about columns */
        int RowSize;                                             /** Size of a single row */
        int Count;                                               /** The number of rows in a table */
        int TableId;                                             /** The table Id */
    } Storage[MAX_TABLE_COUNT];                                  /** Storage information */
    struct WritableSchema                                        /** */
    {                                                            /** */
        struct WritableTableDefinition                           /** */
        {                                                        /** */
            struct WritableTableColDefinition                    /** */
            {                                                    /** */
                char ColumnName[MAX_PROPERTY_NAME_SIZE];         /** The name of the column */
                enum SchemaType ColumnType;                      /** The data type of the column */
                bool ColumnIsNullable;                           /** Is this column nullable */
                bool ColumnIsUnique;                             /** Is this column unique */
                bool ColumnIsPrimaryKey;                         /** Primary key indicator */
                char ColumnDefaultValue[MAX_PROPERTY_NAME_SIZE]; /** We will perform casting according to the type */
                time_t ColumnCreatedAt;                          /** When was the column created */
                time_t ColumnLastModified;                       /** When the column was last modified */
                int ColumnId;                                    /** Id of the column */
            } TableColumnDefs[MAX_TABLE_COLUMN_COUNT];           /** Columns in the table */
            time_t TableCreatedAt;                               /** When was this table created */
            time_t TableLastModified;                            /** When was this table last modified */
            int TablePrimaryKey;                                 /** The column id of the primary key */
            int TableColumnCount;                                /** The number of columns */
            int TableId;                                         /** Id of the table */
            char TableName[MAX_PROPERTY_NAME_SIZE];              /** Name of the table */
        } TableDefs[MAX_TABLE_COUNT];                            /**Tables */
        int TableCount;                                          /** Number of tables in database */
        char SchemaTag[MAX_PROPERTY_NAME_SIZE];                  /** Unique name for schema  */
    } Schema;                                                    /** The database schema */
    time_t FileCreatedAt;                                        /** When was this database created */
    time_t FileLastModified;                                     /** When was this last modified */
    int OffsetImweb;                                             /** Offset values*/
} WritableFileMetadata;
#pragma pack(0)

void FreeFileMetadata(FileMetadata *);
void IntrospectMetadata(const FileMetadata *);

int WriteMetadataToFile(FILE *, FileMetadata *, int(const FileMetadata *, WritableFileMetadata *));
int ReadMetadataFromFile(FILE *, FileMetadata **, FileMetadata *(const WritableFileMetadata *));

FileMetadata *NewFileMetadata(Offset *, SchemaDefinition *, StorageMeta *);
FileMetadata *CreateMetadataFromWritable(const WritableFileMetadata *);
FileMetadata *CreateNewFileMetadataFromSchema(SchemaDefinition *);

int CreateWritableFromMetadata(const FileMetadata *, WritableFileMetadata *);

#endif