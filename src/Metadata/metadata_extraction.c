#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "metadata_extraction.h"
#include "metadata_offsets.h"

int BuildSchemaFromPersistedFormat(FileMetadata *meta, const PersistedFileMetadata *file)
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

    meta->Schema->CreatedAt = file->Schema.CreatedAt;
    meta->Schema->LastModified = file->Schema.LastModified;
    meta->Schema->TagName = malloc(strlen(file->Schema.TagName) + 1);
    if (!meta->Schema->TagName)
    {
        free(meta->Schema);
        fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
        return -1;
    }
    strcpy(meta->Schema->TagName, &file->Schema.TagName);

    meta->Schema->TableDefs = malloc(sizeof(TableDefinition *) * file->TableCount);
    if (!meta->Schema->TableDefs)
    {
        free(meta->Schema);
        free(meta->Schema->TagName);
        fprintf(stderr, "(build-schema-from-persisted-format) malloc failed. \n");
        return -1;
    }

    for (int ti = 0; ti < file->TableCount; ti++)
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

        for (int ci = 0; ci < file->Schema.TableDefs[ti].ColumnCount; ci++)
        {
            int converted;
            TableColDefinition *def;

            def = malloc(sizeof(TableColDefinition));
            if (!def)
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

            def->ColumnName = malloc(strlen(file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnName) + 1);
            if (!def->ColumnName)
            {
                free(def);
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
            strcpy(def->ColumnName, &file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnName);

            converted = ConvertDefaultValue(def, file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnType, file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnDefaultValue);
            if (converted != 0)
            {
                free(def);
                free(def->ColumnName);
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

            def->ColumnCreatedAt = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnCreatedAt;
            def->ColumnIsUnique = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnIsUnique;
            def->ColumnLastModified = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnLastModified;
            def->ColumnType = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnType;
            tableDef->Columns[ci] = def;
        }

        tableDef->ColumnCount = file->Schema.TableDefs[ti].ColumnCount;
        meta->Schema->TableDefs[ti] = tableDef;
    }

    return 0;
}

int ConvertDefaultValue(TableColDefinition *def, enum SchemaType schemaType, const char *defaultValue)
{
    if (!def || !defaultValue)
    {
        fprintf(stderr, "(convert-default-value-err) malloc failed. \n");
        return -1;
    }

    switch (schemaType)
    {
    case DATE:
        def->ColumnDefaultValue = malloc(sizeof(time_t));
        if (def->ColumnDefaultValue)
        {
            *(time_t *)def->ColumnDefaultValue = atol(defaultValue);
            return 0;
        }
        break;
    case INTEGER:
        def->ColumnDefaultValue = malloc(sizeof(int));
        if (def->ColumnDefaultValue)
        {
            *(int *)def->ColumnDefaultValue = atoi(defaultValue);
            return 0;
        }
        break;
    case STRING:
        def->ColumnDefaultValue = malloc(strlen(defaultValue) + 1);
        if (def->ColumnDefaultValue)
        {
            strcpy(def->ColumnDefaultValue, defaultValue);
            return 0;
        }
        break;
    case FLOAT:
        def->ColumnDefaultValue = malloc(sizeof(double));
        if (def->ColumnDefaultValue)
        {
            *(double *)def->ColumnDefaultValue = atof(defaultValue);
            return 0;
        }
        break;
    case BOOL:
        def->ColumnDefaultValue = malloc(sizeof(bool));
        if (def->ColumnDefaultValue)
        {
            *(bool *)def->ColumnDefaultValue = strcmp(defaultValue, &"0") ? true : false;
            return 0;
        }
        break;
    default:
        break;
    }

    return -1;
}

int BuildOffsetsFromPersistedFormat(FileMetadata *meta, const PersistedFileMetadata *file)
{
    TableOffset **offsets;

    if (!meta || !file)
    {
        fprintf(stderr, "(build-offsets-from-persisted-format-err) file or meta is null. \n");
        return -1;
    }

    meta->Offset = malloc(sizeof(Offset));
    if (!meta->Offset)
    {
        fprintf(stderr, "(build-offsets-from-persisted-format-err) malloc failed. \n");
        return -1;
    }

    offsets = malloc(sizeof(TableOffset *) * file->OffsetCount);
    if (!offsets)
    {
        free(meta->Offset);
        fprintf(stderr, "(build-offsets-from-persisted-format-err) malloc failed. \n");
        return -1;
    }

    for (int c = 0; c < file->OffsetCount; c++)
    {
        offsets[c] = NewTableOffset(file->Offsets[c].TableName, file->Offsets[c].Offset);
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
    meta->Offset->ImwebOffset = file->ImwebOffset;
    meta->Offset->OffsetCount = file->OffsetCount;

    return 0;
}

FileMetadata *BootFileMetadataFromFile(const PersistedFileMetadata *metadata)
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

    if (BuildOffsetsFromPersistedFormat(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-metadata-from-file-err) failed while copying over offsets. \n");
        return NULL;
    }

    if (BuildSchemaFromPersistedFormat(target, metadata) != 0)
    {
        free(target);
        fprintf(stderr, "(boot-file-from-persisted-format-err) failed while copying over schema. \n");
        return NULL;
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;

    IntrospectMetadata(target);
    return target;
}

PersistedFileMetadata *MapToPersistedMetadata(const FileMetadata *metadata)
{
    PersistedFileMetadata *target;

    if (!metadata)
    {
        fprintf(stderr, "(map-to-persited-metadata-err) malloc failed \n");
        return NULL;
    }

    target = malloc(sizeof(PersistedFileMetadata));
    if (!target)
    {
        fprintf(stderr, "(map-to-persited-metadata-err) malloc failed \n");
        return NULL;
    }

    for (int count = 0; count < metadata->Schema->TableCount; count++)
    {
        target->Offsets[count].Offset = metadata->Offset->Offsets[count]->Offset;
        strcpy(target->Offsets[count].TableName, metadata->Offset->Offsets[count]->TableName);
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->Schema->TableCount;
    target->ImwebOffset = metadata->Offset->ImwebOffset;

    return target;
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

int WriteMetadataToFile(FILE *file, FileMetadata *in, PersistedFileMetadata *map(const FileMetadata *metadata))
{
    int status = -1;
    PersistedFileMetadata *target;

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

        if (fwrite(target, sizeof(PersistedFileMetadata), 1, file) != 0)
        {
            fprintf(stderr, "(write-metadata-to-file-err) fwrite failed \n");
            break;
        }

        status = 0;
    } while (0);

    free(target);
    return status;
}

int ReadMetadataFromFile(FILE *file, FileMetadata **out, FileMetadata *map(const PersistedFileMetadata *metadata))
{
    PersistedFileMetadata *buffer;
    int status = -1;

    if (!map || !out || !file)
    {
        fprintf(stderr, "(read-metadata-from-file-err) Either `map`, `file`, or `out` could be null \n");
        return -1;
    }

    do
    {
        buffer = malloc(sizeof(PersistedFileMetadata));
        if (!buffer)
        {
            fprintf(stderr, "(read-metadata-from-file-err) malloc failed \n");
            break;
        }

        fread(buffer, sizeof(PersistedFileMetadata), 1, file);
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