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
        fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. passed a null value. \n");
        return -1;
    }

    meta->Schema = malloc(sizeof(SchemaDefinition));
    if (!meta->Schema)
    {
        fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
        return -1;
    }

    meta->Schema->CreatedAt = file->Schema.CreatedAt;
    meta->Schema->LastModified = file->Schema.LastModified;
    meta->Schema->TagName = malloc(strlen(file->Schema.TagName + 1));
    if (!meta->Schema->TagName)
    {
        free(meta->Schema);
        fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
        return -1;
    }
    strcpy(meta->Schema->TagName, &file->Schema.TagName);

    meta->Schema->TableDefs = malloc(sizeof(TableDefinition *) * file->TableCount);
    if (!meta->Schema->TableDefs)
    {
        free(meta->Schema);
        free(meta->Schema->TagName);
        fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
        return -1;
    }

    for (int ti = 0; ti < file->TableCount; ti++)
    {
        TableDefinition *tableDef = malloc(sizeof(TableDefinition));
        if (!tableDef)
        {
            free(meta->Schema->TableDefs);
            free(meta->Schema->TagName);
            free(meta->Schema);
            fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
            return -1;
        }

        for (int ci = 0; ci < file->Schema.TableDefs[ti].ColumnCount; ci++)
        {
            TableColDefinition *def = malloc(sizeof(TableColDefinition));
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
                fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
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
                fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
                return -1;
            }
            strcpy(def->ColumnName, &file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnName);

            int converted = ConvertDefaultValue(def, file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnType, file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnDefaultValue);
            if (!converted)
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
                fprintf(stderr, "[BuildSchemaFromPersistedFormat] Cannot build the schema from persisted file. malloc failed. \n");
                return -1;
            }

            tableDef->Columns[ci] = def;
            def->ColumnCreatedAt = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnCreatedAt;
            def->ColumnIsUnique = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnIsUnique;
            def->ColumnLastModified = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnLastModified;
            def->ColumnType = file->Schema.TableDefs[ti].ColumnDefs[ci].ColumnType;
        }
        tableDef->ColumnCount = file->Schema.TableDefs[ti].ColumnCount;
        meta->Schema->TableDefs[ti] = tableDef;
    }

    return 0;
}

const int ConvertDefaultValue(TableColDefinition *def, const enum SchemaType schemaType, const void *defaultValue)
{
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
            *(bool *)def->ColumnDefaultValue = defaultValue == "0" ? true : false;
            return 0;
        }
        break;
    default:
        break;
    }

    return -1;
}

const int BuildOffsetsFromPersistedFormat(FileMetadata *meta, const PersistedFileMetadata *file)
{
    TableOffset **offsets;

    meta->Offset = malloc(sizeof(Offset));
    if (!meta->Offset)
    {
        fprintf(stderr, "[BuildOffsetsFromPersistedFormat] Error building metadata from persisted format. malloc failed. \n");
        return -1;
    }

    offsets = malloc(sizeof(TableOffset *) * file->OffsetCount);
    if (!offsets)
    {
        free(meta->Offset);
        fprintf(stderr, "[BuildOffsetsFromPersistedFormat] Error building metadata from persisted format. malloc failed. \n");
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
            fprintf(stderr, "[BuildOffsetsFromPersistedFormat] Error building metadata from persisted format. couldn't create table offset. \n");
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
        fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. malloc failed. \n");
        return NULL;
    }

    if (!BuildOffsetsFromPersistedFormat(target, metadata))
    {
        free(target);
        fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. failed while copying over offsets. \n");
        return NULL;
    }

    if (!BuildSchemaFromPersistedFormat(target, metadata))
    {
        free(target);
        fprintf(stderr, "[BootFileMetadataFromFile] Error booting metadata from file. failed while copying over schema. \n");
        return NULL;
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->TableCount;

    IntrospectMetadata(target);
    return target;
}

PersistedFileMetadata *MapToPersistedMetadata(const FileMetadata *metadata)
{
    PersistedFileMetadata *target;

    if (!metadata)
    {
        fprintf(stderr, "[MapToPersistedMetadata] Mapping to persisted metadata failed. malloc failed. \n");
        return NULL;
    }

    target = malloc(sizeof(PersistedFileMetadata));
    if (!target)
    {
        fprintf(stderr, "[MapToPersistedMetadata] Mapping to persisted metadata failed. malloc failed. \n");
        return NULL;
    }

    for (int count = 0; count < metadata->TableCount; count++)
    {
        target->Offsets[count].Offset = metadata->Offset->Offsets[count]->Offset;
        strcpy(target->Offsets[count].TableName, metadata->Offset->Offsets[count]->TableName);
    }

    target->CreatedAt = metadata->CreatedAt;
    target->LastModified = metadata->LastModified;
    target->TableCount = metadata->TableCount;
    target->ImwebOffset = metadata->Offset->ImwebOffset;

    return target;
}

FileMetadata *NewFileMetadata(Offset *offset, SchemaDefinition *schema)
{
    FileMetadata *fMetadata;
    time_t now;

    fMetadata = malloc(sizeof(FileMetadata));
    if (fMetadata == NULL)
    {
        fprintf(stderr, "[NewFileMetadata] cannot create metadata. malloc failed. \n");
        return NULL;
    }

    if (!time(&now))
    {
        fprintf(stderr, "[NewFileMetadata] cannot create metadata. malloc failed. \n");
        free(fMetadata);
        return NULL;
    }

    fMetadata->Offset = offset;
    fMetadata->CreatedAt = now;
    fMetadata->LastModified = now;

    return fMetadata;
}

void FreeFileMetadata(FileMetadata *metadata)
{
    if (metadata == NULL)
    {
        return;
    }

    FreeOffset(metadata->Offset);
    free(metadata);
}

void IntrospectMetadata(const FileMetadata *metadata)
{
    if (!metadata)
    {
        fprintf(stderr, "[IntrospectMetadata] Couldn't introspect metadata. metadata is null.\n");
        return NULL;
    }

    fprintf(stdout, "[IntrospectMetadata] ############# Start of log #############\n\
                                                                                  \n");
    fprintf(stdout, "immediate-write-buffer-offset = %s\n", metadata->Offset->ImwebOffset);
    fprintf(stdout, "created-at                    = %s\n", metadata->CreatedAt);
    fprintf(stdout, "last-modified                 = %s\n", metadata->LastModified);
    fprintf(stdout, "table-count                   = %d\n", metadata->TableCount);
    fprintf(stdout, "offsets                       = \n");
    for (int c = 0; c < metadata->TableCount; c++)
    {
        char *formatted = FormatTableOffset(metadata->Offset->Offsets[c]);
        if (formatted)
        {
            fprintf(stdout, "\t%s\n", formatted);
        }
    }
    fprintf(stdout, "[IntrospectMetadata] ############# End of log #############\n\n");
}

int WriteMetadataToFile(FILE *file, FileMetadata *in, PersistedFileMetadata *map(const FileMetadata *metadata))
{
    int status = -1;
    PersistedFileMetadata *target;
    if (!map)
    {
        fprintf(stderr, "[WriteMetadataToFile] Could not write metadata to file. map is null \n");
        return status;
    }

    do
    {
        target = map(in);
        if (!map)
        {
            fprintf(stderr, "[WriteMetadataToFile] Could not write metadata to file. mapping to persisted format failed \n");
            break;
        }

        if (!fwrite(target, sizeof(PersistedFileMetadata), 1, file))
        {
            fprintf(stderr, "[WriteMetadataToFile] Could not write metadata to file. fwrite failed \n");
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
    if (map == NULL)
    {
        fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. map is null \n");
    }

    do
    {
        buffer = malloc(sizeof(PersistedFileMetadata));
        if (!buffer)
        {
            fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. malloc failed \n");
            break;
        }

        fread(buffer, sizeof(PersistedFileMetadata), 1, file);
        if (!buffer)
        {
            fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. fread failed \n");
            break;
        }

        *out = map(buffer);
        if (!out)
        {
            fprintf(stderr, "[ReadMetadataFromFile] Could not read metadata from file. mapping from persisted format failed  \n");
        }

        status = 0;
    } while (0);

    free(buffer);
    return status;
}