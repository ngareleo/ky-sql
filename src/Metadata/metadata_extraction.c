#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metadata_extraction.h"
#include "metadata_offsets.h"

int ReadSchemaFromWritable(FileMetadata *meta, const WritableFileMetadata *file)
{
    if (!file || !meta)
    {
        fprintf(stderr, "(build-schema-from-persisted-format) passed a null value. \n");
        return -1;
    }

    meta->Schema = malloc(sizeof(SchemaDefinition));
    if (!meta->Schema)
    {
        fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
        return -1;
    }

    meta->Schema->TagName = malloc(strlen(file->Schema.SchemaTag) + 1);
    if (!meta->Schema->TagName)
    {
        free(meta->Schema);
        fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
        return -1;
    }

    meta->Schema->TableDefs = malloc(sizeof(TableDefinition *) * file->Schema.TableCount);
    if (!meta->Schema->TableDefs)
    {
        free(meta->Schema);
        free(meta->Schema->TagName);
        fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
        return -1;
    }

    for (int ti = 0; ti < file->Schema.TableCount; ti++)
    {
        TableDefinition *tableDef;

        tableDef = malloc(sizeof(TableDefinition));
        if (!tableDef)
        {
            for (int ti2 = 0; ti2 < ti; ti2++)
            {
                FreeTableDefinition(meta->Schema->TableDefs[ti2]);
            }
            free(meta->Schema->TableDefs);
            free(meta->Schema->TagName);
            free(meta->Schema);
            fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
            return -1;
        }

        tableDef->Name = malloc(strlen(file->Schema.TableDefs[ti].TableName) + 1);
        if (!tableDef->Name)
        {
            free(tableDef);
            for (int ti2 = 0; ti2 < ti; ti2++)
            {
                FreeTableDefinition(meta->Schema->TableDefs[ti2]);
            }
            free(meta->Schema->TableDefs);
            free(meta->Schema->TagName);
            free(meta->Schema);
            fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
            return -1;
        }

        for (int ci = 0; ci < file->Schema.TableDefs[ti].TableColumnCount; ci++)
        {
            int converted;
            TableColDefinition *colDef;

            colDef = malloc(sizeof(TableColDefinition));
            if (!colDef)
            {
                free(tableDef);
                for (int ti2 = 0; ti2 < ti; ti2++)
                {
                    FreeTableDefinition(meta->Schema->TableDefs[ti2]);
                }
                free(meta->Schema->TableDefs);
                free(meta->Schema->TagName);
                free(meta->Schema);
                fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
                return -1;
            }

            colDef->Name = malloc(strlen(file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName) + 1);
            if (!colDef->Name)
            {
                free(colDef);
                free(tableDef);
                for (int ti2 = 0; ti2 < ti; ti2++)
                {
                    FreeTableDefinition(meta->Schema->TableDefs[ti2]);
                }
                free(meta->Schema->TableDefs);
                free(meta->Schema->TagName);
                free(meta->Schema);
                fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
                return -1;
            }

            strcpy(colDef->Name, &file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName);
            colDef->DefaultValue = &file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue;
            colDef->CreatedAt = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnCreatedAt;
            colDef->Id = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnId;
            colDef->IsUnique = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsUnique;
            colDef->IsNullable = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable;
            colDef->IsPrimaryKey = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable;
            colDef->LastModified = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnLastModified;
            colDef->Type = file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnType;
            tableDef->Columns[ci] = colDef;
        }
        tableDef->PrimaryKeyId = file->Schema.TableDefs[ti].TablePrimaryKey;
        tableDef->Id = file->Schema.TableDefs[ti].TableId;
        tableDef->ColumnCount = file->Schema.TableDefs[ti].TableColumnCount;
        tableDef->CreatedAt = file->Schema.TableDefs[ti].TableCreatedAt;
        tableDef->LastModified = file->Schema.TableDefs[ti].TableLastModified;
        strcpy(tableDef->Name, &file->Schema.TableDefs[ti].TableName);
        meta->Schema->TableDefs[ti] = tableDef;
    }
    meta->Schema->TableCount = file->Schema.TableCount;
    meta->Schema->CreatedAt = file->FileCreatedAt;
    meta->Schema->LastModified = file->FileLastModified;
    strcpy(meta->Schema->TagName, &file->Schema.SchemaTag);

    return 0;
}

void *CreateDefaultValue(enum SchemaType schemaType, const char *defaultValue)
{
    void *output;
    if (!defaultValue)
    {
        fprintf(stderr, "(create-default-value-err) `defaultValue` is null. \n");
        return -1;
    }

    switch (schemaType)
    {
    case DATE:
        output = malloc(sizeof(time_t));
        if (output)
        {
            *(time_t *)output = atol(defaultValue);
            return output;
        }
        break;

    case INTEGER:
        output = malloc(sizeof(int));
        if (output)
        {
            *(int *)output = atoi(defaultValue);
            return output;
        }
        break;

    case STRING:
        output = malloc(strlen(defaultValue) + 1);
        if (output)
        {
            strcpy(output, defaultValue);
            return output;
        }
        break;

    case FLOAT:
        output = malloc(sizeof(double));
        if (output)
        {
            *(double *)output = atof(defaultValue);
            return output;
        }
        break;

    case BOOL:
        output = malloc(sizeof(bool));
        if (output)
        {
            *(bool *)output = strcmp(defaultValue, &"0") ? true : false;
            return output;
        }
        break;

    default:
        break;
    }

    fprintf(stderr, "(create-default-value-err) malloc failed. \n");
    return NULL;
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
        return NULL;
    }

    target = malloc(sizeof(FileMetadata));
    if (!target)
    {
        fprintf(stderr, "(boot-file-metadata-from-file-err) malloc failed. \n");
        return NULL;
    }

    if (ReadOffsetFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-metadata-from-file-err) failed while copying over offsets. \n");
        return NULL;
    }

    if (ReadSchemaFromWritable(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-from-persisted-format-err) failed while copying over schema. \n");
        return NULL;
    }

    target->CreatedAt = metadata->FileCreatedAt;
    target->LastModified = metadata->FileLastModified;

    IntrospectMetadata(target);
    return target;
}

WritableFileMetadata *CreateWritableFromMetadata(const FileMetadata *metadata)
{
    WritableFileMetadata *target;

    if (!metadata)
    {
        fprintf(stderr, "(map-to-persited-metadata-err) malloc failed \n");
        return NULL;
    }

    target = malloc(sizeof(WritableFileMetadata));
    if (!target)
    {
        fprintf(stderr, "(map-to-persited-metadata-err) malloc failed \n");
        return NULL;
    }

    if (WriteOffset(target, metadata) != 0)
    {
        fprintf(stderr, "(map-to-persited-metadata-err) malloc failed \n");
        return NULL;
    }

    return target;
}

// Does not allocate memory
// Assumes memory is pre-allocated
int WriteOffset(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-offset-err) `meta` is null\n");
        return -1;
    }

    file->OffsetImweb = meta->Offset->ImwebOffset;

    /**
     * Here I'm setting a hard rule that number of table offsets are always equal to the number of tables.
     * They're represented by different structs
     */
    for (int ti = 0; ti < meta->Schema->TableCount; ti++)
    {
        file->TableOffsets[ti].TableOffset = meta->Offset->Offsets[ti]->Offset;
        file->TableOffsets[ti].TableOffsetId = meta->Offset->Offsets[ti]->Id;
    }
}

int WriteSchema(WritableFileMetadata *file, const FileMetadata *meta)
{
    if (!meta)
    {
        fprintf(stderr, "(write-schema-err) meta is null \n");
        return -1;
    }

    for (int ti = 0; ti < meta->Schema->TableCount; ti++)
    {
        for (int ci = 0; ci < meta->Schema->TableDefs[ci]->ColumnCount; ci++)
        {
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnId = meta->Schema->TableDefs[ti]->Columns[ci]->Id;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnCreatedAt = meta->Schema->TableDefs[ti]->Columns[ci]->CreatedAt;
            strcpy(&file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnDefaultValue, meta->Schema->TableDefs[ti]->Columns[ci]->DefaultValue);
            strcpy(&file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnName, meta->Schema->TableDefs[ti]->Columns[ci]->Name);
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsNullable = meta->Schema->TableDefs[ti]->Columns[ci]->IsNullable;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsPrimaryKey = meta->Schema->TableDefs[ti]->Columns[ci]->IsPrimaryKey;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnIsUnique = meta->Schema->TableDefs[ti]->Columns[ci]->IsUnique;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnLastModified = meta->Schema->TableDefs[ti]->Columns[ci]->LastModified;
            file->Schema.TableDefs[ti].TableColumnDefs[ci].ColumnType = meta->Schema->TableDefs[ti]->Columns[ci]->Type;
        }
        file->Schema.TableDefs[ti].TableColumnCount = meta->Schema->TableDefs[ti]->ColumnCount;
        file->Schema.TableDefs[ti].TableCreatedAt = meta->Schema->TableDefs[ti]->CreatedAt;
        file->Schema.TableDefs[ti].TableId = meta->Schema->TableDefs[ti]->Id;
        file->Schema.TableDefs[ti].TablePrimaryKey = meta->Schema->TableDefs[ti]->PrimaryKeyId;
        file->Schema.TableDefs[ti].TableLastModified = meta->Schema->TableDefs[ti]->LastModified;
        strcpy(&file->Schema.TableDefs[ti].TableName, meta->Schema->TableDefs[ti]->LastModified);
    }
    strcpy(&file->Schema.SchemaTag, meta->Schema->TagName);
    file->Schema.TableCount = meta->Schema->TableCount;
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
    fprintf(stdout, "\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "(log) created-at                      = %ld.\n", metadata->CreatedAt);
    fprintf(stdout, "(log) last-modified                   = %ld.\n", metadata->LastModified);
    fprintf(stdout, "\n");
    fprintf(stdout, "############# Start of offsets log #############\n");
    fprintf(stdout, "%s\n", FormatOffset(metadata->Offset));
    fprintf(stdout, "############# End of offsets log   #############\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "############# Start of schema log  #############\n");
    fprintf(stdout, "%s\n", FormatSchemaDefinition(metadata->Schema));
    fprintf(stdout, "############# End of schema log    #############\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "############# End of metadata log  #############\n");
}

int WriteMetadataToFile(FILE *file, FileMetadata *in, WritableFileMetadata *map(const FileMetadata *metadata))
{
    int status = -1;
    WritableFileMetadata *target;

    if (!map || !file || !in)
    {
        fprintf(stderr, "(write-metadata-to-file-err) Either `map`, `file`, or `in` could be null \n");
        return status;
    }

    do
    {
        target = map(in);
        if (!target)
        {
            fprintf(stderr, "(write-metadata-to-file-err) mapping to persisted format failed \n");
            break;
        }

        if (fwrite(target, sizeof(WritableFileMetadata), 1, file) != 0)
        {
            fprintf(stderr, "(write-metadata-to-file-err) fwrite failed \n");
            break;
        }

        status = 0;
    } while (0);

    free(target);
    return status;
}

int ReadMetadataFromFile(FILE *file, FileMetadata **out, FileMetadata *map(const WritableFileMetadata *metadata))
{
    WritableFileMetadata *buffer;
    int status = -1;

    if (!map || !out || !file)
    {
        fprintf(stderr, "(read-metadata-from-file-err) Either `map`, `file`, or `out` could be null \n");
        return -1;
    }

    do
    {
        buffer = malloc(sizeof(WritableFileMetadata));
        if (!buffer)
        {
            fprintf(stderr, "(read-metadata-from-file-err) malloc failed \n");
            break;
        }

        fread(buffer, sizeof(WritableFileMetadata), 1, file);
        if (!buffer)
        {
            fprintf(stderr, "(read-metadata-from-file-err) fread failed \n");
            break;
        }

        *out = map(buffer);
        if (!out)
        {
            fprintf(stderr, "(read-metadata-from-file-err) mapping from persisted format failed \n");
        }

        status = 0;
    } while (0);

    free(buffer);
    return status;
}