#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "include/metadata_extraction.h"
#include "include/metadata_offsets.h"
#include "include/metadata_default.h"
#include "../Utilities/utilities.h"

FileMetadata *CreateNewFileMetadataFromSchema(SchemaDefinition *schema)
{
    FileMetadata *meta;
    Offset *offset;

    if (!schema)
    {
        fprintf(stderr, "(create-new-file-metadata-err) could not create schema\n");
        return NULL;
    }

    offset = NewOffset(0); // A default imweb of 0
    if (!offset)
    {
        fprintf(stderr, "(create-new-file-metadata-err) could not create offset\n");
        return NULL;
    }

    fprintf(stdout, "(create-new-file-metadata-info) offset creation success\n");
    fprintf(stderr, "(create-new-file-metadata-log) schema table count --> %d \n", schema->TableCount);
    for (int ti = 0; ti < schema->TableCount; ti++)
    {

        // Todo: Optimize this. AddTableOffset is not suitable for batch updates
        // Todo: Since we know the table count ahead of time, we don't have to link a table
        // Todo: One by one, we can do one allocation
        TableOffset *tableOff = NewTableOffset(
            schema->TableDefs[ti]->Id,
            0 // Todo: Compute this value based on real offsets
        );
        if (AddTableOffset(offset, tableOff) != 0)
        {
            FreeOffset(offset);
            fprintf(stderr, "(create-new-file-metadata-err) could not add table offset\n");
            return NULL;
        }
    }

    fprintf(stdout, "(create-new-file-metadata-info) table offsets linked\n");
    meta = NewFileMetadata(offset, schema);
    if (!meta)
    {
        FreeOffset(offset);
        fprintf(stderr, "(create-new-file-metadata-err) could not create metadata\n");
        return NULL;
    }

    return meta;
}

int ReadSchemaFromWritable(FileMetadata *meta, const WritableFileMetadata *file)
{
    if (!file || !meta)
    {
        fprintf(stderr, "(read-schema-from-writable) passed a null value. \n");
        return -1;
    }

    meta->Schema = malloc(sizeof(SchemaDefinition));
    if (!meta->Schema)
    {
        fprintf(stderr, "(read-schema-from-writable-err) malloc failed. \n");
        return -1;
    }

    meta->Schema->TagName = malloc(strlen(file->Schema.SchemaTag) + 1);
    if (!meta->Schema->TagName)
    {
        free(meta->Schema);
        fprintf(stderr, "(read-schema-from-writable-err) malloc failed. \n");
        return -1;
    }

    meta->Schema->TableDefs = malloc(sizeof(TableDefinition *) * file->Schema.TableCount);
    if (!meta->Schema->TableDefs)
    {
        free(meta->Schema);
        free(meta->Schema->TagName);
        fprintf(stderr, "(read-schema-from-writable-err) malloc failed. \n");
        return -1;
    }

    for (int ti = 0; ti < file->Schema.TableCount; ti++)
    {
        fprintf(stdout, "(read-schema-from-writable-info) 1 starting table copy \n");
        TableDefinition *tableDef;
        tableDef = malloc(sizeof(TableDefinition));
        if (!tableDef)
        {
            fprintf(stderr, "(read-schema-from-writable) malloc failed. \n");
            for (int ti2 = 0; ti2 < ti; ti2++)
            {
                FreeTableDefinition(meta->Schema->TableDefs[ti2]);
            }
            free(meta->Schema->TableDefs);
            free(meta->Schema->TagName);
            free(meta->Schema);
            return -1;
        }

        tableDef->Name = malloc(strlen(file->Schema.TableDefs[ti].TableName) + 1);
        if (!tableDef->Name)
        {
            fprintf(stderr, "(read-schema-from-writable) malloc failed. \n");
            free(tableDef);
            for (int ti2 = 0; ti2 < ti; ti2++)
            {
                FreeTableDefinition(meta->Schema->TableDefs[ti2]);
            }
            free(meta->Schema->TableDefs);
            free(meta->Schema->TagName);
            free(meta->Schema);
            return -1;
        }

        tableDef->Columns = malloc(sizeof(TableColDefinition) * file->Schema.TableDefs[ti].TableColumnCount);
        if (!tableDef->Columns)
        {
            fprintf(stderr, "(read-schema-from-writable) malloc failed. \n");
            free(tableDef->Name);
            free(tableDef);
            for (int ti2 = 0; ti2 < ti; ti2++)
            {
                FreeTableDefinition(meta->Schema->TableDefs[ti2]);
            }
            free(meta->Schema->TableDefs);
            free(meta->Schema->TagName);
            free(meta->Schema);
            return -1;
        }

        for (int ci = 0; ci < file->Schema.TableDefs[ti].TableColumnCount; ci++)
        {
            fprintf(stdout, "(read-schema-from-writable-info) 2 starting table column copy \n");
            TableColDefinition *colDef;
            colDef = malloc(sizeof(TableColDefinition));
            if (!colDef)
            {
                fprintf(stderr, "(read-schema-from-writable) malloc failed. \n");
                free(tableDef->Name);
                free(tableDef);
                for (int ti2 = 0; ti2 < ti; ti2++)
                {
                    FreeTableDefinition(meta->Schema->TableDefs[ti2]);
                }
                free(meta->Schema->TableDefs);
                free(meta->Schema->TagName);
                free(meta->Schema);
                return -1;
            }

            colDef->Name = malloc(strlen(file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName) + 1);
            if (!colDef->Name)
            {
                fprintf(stderr, "(read-schema-from-writable) malloc failed. \n");
                free(colDef);
                free(tableDef);
                for (int ti2 = 0; ti2 < ti; ti2++)
                {
                    FreeTableDefinition(meta->Schema->TableDefs[ti2]);
                }
                free(meta->Schema->TableDefs);
                free(meta->Schema->TagName);
                free(meta->Schema);
                return -1;
            }

            // !! For now we treat default values as character array
            // !! The way we handle default values will be different for each data type

            if (strlen(file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue) > 0)
            {

                colDef->DefaultValue = malloc(strlen(file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue) + 1);
                if (!colDef->DefaultValue)
                {
                    fprintf(stderr, "(read-schema-from-writable) malloc failed. \n");
                    free(colDef);
                    free(tableDef->Name);
                    free(tableDef);
                    for (int ti2 = 0; ti2 < ti; ti2++)
                    {
                        FreeTableDefinition(meta->Schema->TableDefs[ti2]);
                    }
                    free(meta->Schema->TableDefs);
                    free(meta->Schema->TagName);
                    free(meta->Schema);
                    return -1;
                }
                else
                {

                    strcpy(colDef->DefaultValue, &file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue);
                }
            }

            strcpy(colDef->Name, &file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName);
            colDef->CreatedAt = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnCreatedAt;
            colDef->Id = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnId;
            colDef->IsUnique = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsUnique;
            colDef->IsNullable = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable;
            colDef->IsPrimaryKey = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsPrimaryKey;
            colDef->LastModified = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnLastModified;
            colDef->Type = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnType;
            tableDef->Columns[ci] = colDef;
            fprintf(stderr, "(read-schema-from-writable-info) 2 table column copied over \n");
        }
        tableDef->PrimaryKeyId = file->Schema.TableDefs[ti].TablePrimaryKey;
        tableDef->Id = file->Schema.TableDefs[ti].TableId;
        tableDef->ColumnCount = file->Schema.TableDefs[ti].TableColumnCount;
        tableDef->CreatedAt = file->Schema.TableDefs[ti].TableCreatedAt;
        tableDef->LastModified = file->Schema.TableDefs[ti].TableLastModified;
        strcpy(tableDef->Name, &file->Schema.TableDefs[ti].TableName);
        meta->Schema->TableDefs[ti] = tableDef;
        fprintf(stderr, "(read-schema-from-writable-info) 1 table  copied over \n");
    }
    meta->Schema->TableCount = file->Schema.TableCount;
    meta->Schema->CreatedAt = file->FileCreatedAt;
    meta->Schema->LastModified = file->FileLastModified;
    strcpy(meta->Schema->TagName, &file->Schema.SchemaTag);

    return 0;
}

int ReadOffsetFromWritable(FileMetadata *meta, const WritableFileMetadata *file)
{
    TableOffset **offsets;

    if (!meta || !file)
    {
        fprintf(stderr, "(build-offsets-from-persisted-format-err) file or meta is null. \n");
        return -1;
    }

    meta->Offset = malloc(sizeof(TableOffset));
    if (!meta->Offset)
    {
        fprintf(stderr, "(build-offsets-from-persisted-format-err) malloc failed. \n");
        return -1;
    }

    offsets = malloc(sizeof(TableOffset *) * file->Schema.TableCount);
    if (!offsets)
    {
        free(meta->Offset);
        fprintf(stderr, "(build-offsets-from-persisted-format-err) malloc failed. \n");
        return -1;
    }

    for (int c = 0; c < file->Schema.TableCount; c++)
    {
        offsets[c] = NewTableOffset(file->TableOffsets[c].TableOffsetId, file->TableOffsets[c].TableOffset);
        if (!offsets[c])
        {
            for (int c2 = 0; c2 < c; c2++)
            {
                free(offsets[c2]);
            }
            free(meta->Offset);
            free(offsets);
            fprintf(stderr, "(build-offsets-from-persisted-format-err) couldn't create table offset. \n");
            return -1;
        }
    }

    meta->Offset->Offsets = offsets;
    meta->Offset->ImwebOffset = file->OffsetImweb;
    meta->Offset->TableCount = file->Schema.TableCount;

    return 0;
}

FileMetadata *CreateMetadataFromWritable(const WritableFileMetadata *metadata)
{
    FileMetadata *target;
    if (!metadata)
    {
        fprintf(stderr, "(create-metadata-from-info) metadata is null. \n");
        return NULL;
    }

    target = malloc(sizeof(FileMetadata));
    if (!target)
    {
        fprintf(stderr, "(create-metadata-from-writable) malloc failed. \n");
        return NULL;
    }

    if (ReadOffsetFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(create-metadata-from-writable) failed while copying over offsets. \n");
        return NULL;
    }
    fprintf(stdout, "(create-metadata-from-writable) offsets copied to metadata . \n");

    if (ReadSchemaFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-from-persisted-format-err) failed while copying over schema. \n");
        return NULL;
    }
    fprintf(stdout, "(create-metadata-from-writable) schema copied to metadata . \n");

    target->CreatedAt = metadata->FileCreatedAt;
    target->LastModified = metadata->FileLastModified;

    IntrospectMetadata(target);
    return target;
}

int CreateWritableFromMetadata(const FileMetadata *metadata, WritableFileMetadata *target)
{
    if (!metadata)
    {
        fprintf(stderr, "(create-writable-from-metadata) metadata is null \n");
        return -1;
    }

    if (!target)
    {
        fprintf(stderr, "(create-writable-from-metadata) target is not pre-allocated \n");
        return -1;
    }

    if (WriteOffset(target, metadata) != 0)
    {
        fprintf(stderr, "(create-writable-from-metadata) failed to write offset \n");
        return -1;
    }
    fprintf(stdout, "(create-writable-from-metadata) offsets copied to writable \n");

    if (WriteSchema(target, metadata) != 0)
    {
        fprintf(stderr, "(create-writable-from-metadata) failed to write schema \n");
        return -1;
    }
    fprintf(stdout, "(create-writable-from-metadata) schema written to writable metadata \n");

    target->FileCreatedAt = metadata->CreatedAt;
    target->FileLastModified = metadata->LastModified;
    target->OffsetImweb = metadata->Offset->ImwebOffset;
    return 0;
}

// !! Assumes memory is pre-allocated
int WriteOffset(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-offset-err) meta is null\n");
        return -1;
    }

    file->OffsetImweb = meta->Offset->ImwebOffset;

    /**
     * !! Here lies Dragons !!
     *
     * Here I'm setting a hard rule that number of table offsets are always equal to the number of tables.
     * Although they're represented by different structs
     * There's no gurantee this holds in instructions upstream
     *
     * Validating seems like a waste for now so I'll keep it this way
     */
    for (int ti = 0; ti < meta->Schema->TableCount; ti++)
    {
        file->TableOffsets[ti].TableOffset = meta->Offset->Offsets[ti]->Offset;
        file->TableOffsets[ti].TableOffsetId = meta->Offset->Offsets[ti]->Id;
    }

    return 0;
}

int WriteSchema(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-schema-err) meta is null \n");
        return -1;
    }

    fprintf(stderr, "(write-schema-log) starting schema copy \n");
    for (int ti = 0; ti < meta->Schema->TableCount; ti++)
    {
        fprintf(stderr, "(write-schema-log) starting table copy \n");
        for (int ci = 0; ci < meta->Schema->TableDefs[ti]->ColumnCount; ci++)
        {
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnId = meta->Schema->TableDefs[ti]->Columns[ci]->Id;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnCreatedAt = meta->Schema->TableDefs[ti]->Columns[ci]->CreatedAt;
            strcpy(&file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName, meta->Schema->TableDefs[ti]->Columns[ci]->Name);
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable = meta->Schema->TableDefs[ti]->Columns[ci]->IsNullable;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsPrimaryKey = meta->Schema->TableDefs[ti]->Columns[ci]->IsPrimaryKey;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsUnique = meta->Schema->TableDefs[ti]->Columns[ci]->IsUnique;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnLastModified = meta->Schema->TableDefs[ti]->Columns[ci]->LastModified;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnType = meta->Schema->TableDefs[ti]->Columns[ci]->Type;
            if (meta->Schema->TableDefs[ti]->Columns[ci]->DefaultValue)
            {
                strcpy(&file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue, meta->Schema->TableDefs[ti]->Columns[ci]->DefaultValue);
            }
        }
        fprintf(stderr, "(write-schema-log) table columns written over \n");
        file->Schema.TableDefs[ti].TableColumnCount = meta->Schema->TableDefs[ti]->ColumnCount;
        file->Schema.TableDefs[ti].TableCreatedAt = meta->Schema->TableDefs[ti]->CreatedAt;
        file->Schema.TableDefs[ti].TableId = meta->Schema->TableDefs[ti]->Id;
        file->Schema.TableDefs[ti].TablePrimaryKey = meta->Schema->TableDefs[ti]->PrimaryKeyId;
        file->Schema.TableDefs[ti].TableLastModified = meta->Schema->TableDefs[ti]->LastModified;
        strcpy(&file->Schema.TableDefs[ti].TableName, meta->Schema->TableDefs[ti]->Name);
        fprintf(stderr, "(write-schema-log) table written over \n");
    }

    strcpy(&file->Schema.SchemaTag, meta->Schema->TagName);
    file->Schema.TableCount = meta->Schema->TableCount;

    return 0;
}

FileMetadata *NewFileMetadata(Offset *offset, SchemaDefinition *schema)
{
    FileMetadata *metadata;
    time_t now;

    if (!offset || !schema)
    {
        fprintf(stderr, "(new-file-metadata-err) offset or schema is null \n");
        return NULL;
    }

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(new-file-metadata-err) now failed \n");
        return NULL;
    }

    metadata = malloc(sizeof(FileMetadata));
    if (!metadata)
    {
        fprintf(stderr, "(new-file-metadata-err) malloc failed \n");
        return NULL;
    }

    metadata->Offset = offset;
    metadata->CreatedAt = now;
    metadata->LastModified = now;
    metadata->Schema = schema;
    fprintf(stderr, "(new-file-metadata-info) file metadata creation success \n");
    return metadata;
}

void FreeFileMetadata(FileMetadata *metadata)
{
    if (metadata)
    {
        FreeOffset(metadata->Offset);
        free(metadata);
    }
}

void IntrospectMetadata(const FileMetadata *metadata)
{
    if (!metadata)
    {
        fprintf(stderr, "(introspect-metadata-err) metadata is NULL.\n");
        return;
    }

    fprintf(stdout, "############# Start of metadata log  #############\n");
    fprintf(stdout, "############# Start of schema log  #############\n");
    fprintf(stdout, "%s\n", NullGuardStr(FormatSchemaDefinition(metadata->Schema)));
    fprintf(stdout, "############# End of schema log    #############\n");
    fprintf(stdout, "(log) created-at                      = %ld.\n", metadata->CreatedAt);
    fprintf(stdout, "(log) last-modified                   = %ld.\n", metadata->LastModified);
    fprintf(stdout, "\n");
    fprintf(stdout, "############# Start of offsets log #############\n");
    fprintf(stdout, "%s\n", NullGuardStr(FormatOffset(metadata->Offset)));
    fprintf(stdout, "############# End of offsets log   #############\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "############# End of metadata log  #############\n");
}

int WriteMetadataToFile(FILE *file, FileMetadata *in, int map(const FileMetadata *metadata, WritableFileMetadata *writable))
{
    int status = -1;
    WritableFileMetadata target;

    if (!map || !file || !in)
    {
        fprintf(stderr, "(write-metadata-to-file-err) Either `map`, `file`, or `in` could be null \n");
        return status;
    }

    do
    {
        status = map(in, &target);
        if (status != 0)
        {
            fprintf(stderr, "(write-metadata-to-file-err) mapping to persisted format failed \n");
            break;
        }

        if (fwrite(&target, sizeof(WritableFileMetadata), 1, file) < 1)
        {
            perror("fwrite");
            fprintf(stderr, "(write-metadata-to-file-err) fwrite failed \n");
            break;
        }
        else if (feof(file) != 0)
        {
            fprintf(stderr, "(write-metadata-to-file-err) EOF reached \n");
            break;
        }

        status = 0;
    } while (0);
    return status;
}

int ReadMetadataFromFile(FILE *file, FileMetadata **out, FileMetadata *map(const WritableFileMetadata *metadata))
{
    WritableFileMetadata *writable;
    int status = -1;

    if (!map || !out || !file)
    {
        fprintf(stderr, "(read-metadata-from-file-err) Either `map`, `file`, or `out` could be null \n");
        return -1;
    }

    do
    {
        writable = malloc(sizeof(WritableFileMetadata));
        if (!writable)
        {
            fprintf(stderr, "(read-metadata-from-file-err) malloc failed \n");
            break;
        }

        int read = fread(writable, sizeof(WritableFileMetadata), 1, file);
        if (read < 1)
        {
            fprintf(stderr, "(read-metadata-from-file-err) fread failed \n");
        }
        else if (!writable)
        {
            fprintf(stderr, "(read-metadata-from-file-err) fread failed. writable is null \n");
            break;
        }
        fprintf(stdout, "(read-metadata-from-file-info) data read from file \n");

        *out = map(writable);
        if (!out)
        {
            fprintf(stderr, "(read-metadata-from-file-err) mapping from persisted format failed \n");
        }
        fprintf(stdout, "(read-metadata-from-file-info) metadata read successfully  \n");

        status = 0;
    } while (0);

    free(writable);
    return status;
}