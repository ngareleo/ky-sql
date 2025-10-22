#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_block_type.h"
#include "../Types/types.h"
#include "../Utilities/utilities.h"

DataBlockType *CreateDataBlock(char **headers, char ***values)
{
    if (!values)
    {
        fprintf(stderr, "(create-data-block) input invalid \n");
        return NULL;
    }

    DataBlockType *block;
    int headerCount, rowCount;

    headerCount = Count((void **)headers);
    rowCount = Count((void **)values);

    block = malloc(sizeof(DataBlockType));
    if (!block)
    {
        fprintf(stderr, "(create-data-block) malloc failed \n");
        return NULL;
    }

    if (headerCount > 0)
    {
        block->Header = malloc(sizeof(char *) * (headerCount + 1));
        if (!block->Header)
        {
            fprintf(stderr, "(create-data-block) malloc failed \n");
            return NULL;
        }

        for (int h_c = 0; h_c < headerCount; h_c++)
        {
            block->Header[h_c] = malloc(strlen(headers[h_c]) + 1);
            if (!block->Header[h_c])
            {
                for (int h_c_1 = 0; h_c_1 < h_c; h_c_1++)
                {
                    free(block->Header[h_c_1]);
                }
                free(block->Header);
                fprintf(stderr, "(create-data-block) malloc failed \n");
                return NULL;
            }
            strcpy(block->Header[h_c], headers[h_c]);
        }

        block->Header[headerCount] = NULL;
    }

    if (rowCount > 0)
    {

        block->Values = malloc(sizeof(char **) * (rowCount + 1));
        if (!block->Values)
        {
            for (int h = 0; h < headerCount; h++)
            {
                free(block->Header[h]);
            }
            free(block->Header);
            fprintf(stderr, "(create-data-block) malloc failed \n");
            return NULL;
        }

        for (int v_c = 0; v_c < rowCount; v_c++)
        {
            int rSize = Count((void **)values[v_c]);
            if (rSize > 0)
            {
                block->Values[v_c] = malloc(sizeof(char **) * rSize + 1);
                if (!block->Values[v_c])
                {
                    for (int h = 0; h < headerCount; h++)
                    {
                        free(block->Header[h]);
                    }
                    free(block->Header);
                    free(block->Values);
                    fprintf(stderr, "(create-data-block) malloc failed \n");
                    return NULL;
                }

                for (int r_c = 0; r_c < rSize; r_c++)
                {
                    block->Values[v_c][r_c] = malloc(strlen(values[v_c][r_c]) + 1);
                    if (!block->Values[v_c][r_c])
                    {
                        // We recover mem held by headers
                        for (int h = 0; h < headerCount; h++)
                        {
                            free(block->Header[h]);
                        }
                        free(block->Header);

                        // We recover mem for all the rows we've allocated in this turn
                        for (int r_c_1 = 0; r_c_1 < r_c; r_c_1++)
                        {
                            free(block->Values[v_c][r_c_1]);
                        }

                        // We recover mem for all the rows we've allocated so far
                        for (int v_c_1 = 0; v_c_1 < v_c; v_c_1++)
                        {
                            for (int r_c_1 = 0; r_c_1 < Count((void **)values[v_c_1]); r_c_1++)
                            {
                                free(block->Values[v_c][r_c_1]);
                            }
                            free(block->Values[v_c_1]);
                        }

                        free(block->Values[v_c]);
                        free(block->Values);
                        fprintf(stderr, "(create-data-block) malloc failed \n");
                        return NULL;
                    }
                    strcpy(block->Values[v_c][r_c], values[v_c][r_c]);
                }

                block->Values[v_c][rSize] = NULL;
            }
        }

        block->Values[rowCount] = NULL;
    }

    ValidateDataBlock(block);
    return block;
}

DataBlockSize *EmptyBlockSize()
{
    DataBlockSize *s;
    s = malloc(sizeof(DataBlockSize));
    if (!s)
    {
        fprintf(stderr, "(empty-block-size) malloc failed \n");
        return NULL;
    }

    s->Count = 0;
    s->Width = 0;
    return s;
}

DataBlockSize *MeasureBlockStructure(char ***rowText)
{
    if (!rowText)
    {
        fprintf(stderr, "(validate-block-structure) Invalid text\n");
        return NULL;
    }

    int count, dimC;
    if ((count = Count((void **)rowText)) < 1)
    {
        return EmptyBlockSize();
    }

    if ((dimC = Count((void **)rowText[0]) < 1))
    {
        return EmptyBlockSize();
    }

    for (int c = 1; c < count; c++)
    {
        int rowC;
        // Whenever the data is unaligned, we use the widest width
        // Should not cause problems as long as we depend on `IsAligned` property before checking width
        if ((rowC = Count((void **)rowText[c])) > dimC)
        {
            dimC = rowC;
        }
    }

    DataBlockSize *s;
    s = malloc(sizeof(DataBlockSize));
    if (!s)
    {
        fprintf(stderr, "(validate-block-structure) malloc failed\n");
        return NULL;
    }

    s->Count = count;
    s->Width = dimC;

    return s;
}

void ValidateDataBlock(DataBlockType *block)
{
    if (!block)
    {
        fprintf(stderr, "(validate-data-block) arg is invalid \n");
        return -1;
    }

    DataBlockSize *size;
    int count, dimC;
    int headerC;

    block->IsValid = 1;
    if (!block->Header || (headerC = Count((void **)block->Header)) < 1)
    {
        block->IsHeaderLess = 1;
    }

    count = Count((void **)block->Values);
    if (count < 1)
    {
        block->IsEmpty = 1;
        block->IsAligned = 1;
        block->Size = EmptyBlockSize();
        return;
    }

    size = MeasureBlockStructure(block->Values);
    if (size)
    {
        fprintf(stderr, "(validate-data-block) couldn't measure data block \n");
        return -1;
    }

    dimC = Count((void **)block->Values[0]);
    if (size->Count == 0 || dimC < 1)
    {
        block->IsEmpty = 1;
        block->IsAligned = 1;

        // !! Here lies dragons
        // !! We purposerfully return an empty block size
        // !! even if there's one empty element
        // !! We treat this case as an empty data block
        // !! So we enforce the count of 0
        // !! I hope we don't come to regret this
        block->Size = EmptyBlockSize();
    }
    else
    {
        block->IsAligned = 1;
        block->Size = size;
        for (int c = 1; c < dimC; c++)
        {
            if (Count((void **)block->Values[c]) != dimC)
            {
                block->IsAligned = 0;
                block->IsValid = 0;
                break;
            }
        }

        if (dimC != count)
        {
            block->IsValid = 0;
        }
    }
}

void FreeDataBlock(DataBlockType *block)
{
    if (block)
    {
        free(block->Size);
        free(block);
    }
}