#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "data_block_type.h"
#include "Types/types.h"
#include "Utilities/utilities.h"

DataBlockType *CreateDataBlock(char **headers, char ***values)
{
    if (!values && !headers)
    {
        return DefaultBlock();
    }

    Allocator *alloc = MallocInit();
    DataBlockType *block;
    int headerCount, rowCount;
    bool allocFailed = false;

    headerCount = Count((void **)headers);
    rowCount = Count((void **)values);

    // ?? Allocation

    block = Malloc(sizeof(DataBlockType), alloc);
    block->Header = Malloc(sizeof(char *) * (headerCount + 1), alloc);
    block->Values = Malloc(sizeof(char **) * (rowCount + 1), alloc);

    if (headerCount > 0)
    {
        for (int h_c = 0; h_c < headerCount; h_c++)
        {
            block->Header[h_c] = Malloc(strlen(headers[h_c]) + 1, alloc);
        }
        block->Header[headerCount] = NULL;
    }

    if (rowCount > 0)
    {
        for (int row_n = 0; row_n < rowCount; row_n++)
        {
            int rSize = Count((void **)values[row_n]);
            if (rSize > 0)
            {
                block->Values[row_n] = Malloc(sizeof(char **) * (rSize + 1), alloc);
                for (int r_val = 0; r_val < rSize; r_val++)
                {
                    block->Values[row_n][r_val] = Malloc(strlen(values[row_n][r_val]) + 1, alloc);
                }
                block->Values[row_n][rSize] = NULL;
            }
        }
        block->Values[rowCount] = NULL;
    }

    if (VerifyAllocation(alloc))
    {
        for (int h_c_1 = 0; h_c_1 < headerCount; h_c_1++)
        {
            free(block->Header[h_c_1]);
        }

        for (int row_n = 0; row_n < rowCount; row_n++)
        {
            int rSize = Count((void **)values[row_n]);
            if (rSize > 0)
            {
                for (int r_val = 0; r_val < rSize; r_val++)
                    free(block->Values[row_n][r_val]);
                free(block->Values[row_n]);
            }
        }

        free(block->Header);
        free(block->Values);
        fprintf(stderr, "(create-data-block) mem allocation failed \n");
        return NULL;
    }

    // ?? Allocation

    if (headerCount > 0)
    {
        for (int h_c = 0; h_c < headerCount; h_c++)
        {
            strcpy(block->Header[h_c], headers[h_c]);
        }
    }
    if (rowCount > 0)
    {
        for (int row_n = 0; row_n < rowCount; row_n++)
        {
            int rSize = Count((void **)values[row_n]);
            if (rSize > 0)
            {
                for (int r_val = 0; r_val < rSize; r_val++)
                {
                    strcpy(block->Values[row_n][r_val], values[row_n][r_val]);
                }
            }
        }
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

DataBlockType *DefaultBlock()
{
    DataBlockType *block;
    block = malloc(sizeof(DataBlockType));
    if (!block)
    {
        fprintf(stderr, "(default-block) malloc failed \n");
        return NULL;
    }

    DataBlockSize *s;
    s = malloc(sizeof(DataBlockSize));
    if (!s)
    {
        fprintf(stderr, "(default-block) malloc failed \n");
        return NULL;
    }

    s->Count = 0;
    s->Width = 0;

    block->Size = s;
    block->Header = NULL;
    block->Values = NULL;
    block->IsAligned = true;
    block->IsEmpty = true;
    block->IsHeaderLess = true;
    block->IsValid = true;

    return block;
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

    // Count the first row and use that as base for default row width
    if ((dimC = Count((void **)rowText[0]) < 1))
    {
        return EmptyBlockSize();
    }

    for (int c = 1; c < count; c++)
    {
        int rowC;
        // !! Whenever the data is unaligned, use the widest width as row width
        // !! Should not cause problems as long as we read `IsAligned` and `IsValid` property before accessing width
        if ((rowC = Count((void **)rowText[c])) > dimC)
        {
            dimC = rowC;
        }
    }

    DataBlockSize *size;
    size = malloc(sizeof(DataBlockSize));
    if (!size)
    {
        fprintf(stderr, "(validate-block-structure) malloc failed\n");
        return NULL;
    }

    size->Count = count;
    size->Width = dimC;
    return size;
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

    block->IsValid = true;

    if (!block->Header ||
        (headerC = Count((void **)block->Header)) < 1)
    {
        block->IsHeaderLess = true;
    }

    if (!block->Values ||
        (count = Count((void **)block->Values)) < 1)
    {
        block->IsEmpty = true;
        block->IsAligned = true;
        block->Size = EmptyBlockSize();
        return;
    }

    size = MeasureBlockStructure(block->Values);
    if (!size)
    {
        fprintf(stderr, "(validate-data-block-err) error measuring data-block \n");
        return;
    }

    dimC = Count((void **)block->Values[0]);
    if (dimC < 1)
    {
        block->IsEmpty = true;
        block->IsAligned = true;

        // !! Here lies dragons
        // !! We purposerfully return an empty block size even if there's one empty element
        // !! We treat this case as an empty data block so we enforce the count of 0 and width of 0
        block->Size = EmptyBlockSize();
        return;
    }

    block->IsAligned = true;
    block->Size = size;
    for (int c = 1; c < dimC; c++)
    {
        if (Count((void **)block->Values[c]) != dimC)
        {
            block->IsAligned = false;

            // ?? A valid data block should be well aligned
            block->IsValid = false;
            break;
        }
    }

    if (dimC != count)
    {
        // ?? We don't set alignment to false because alignment is a values construct
        // ?? but we mark the data block as invalid because headers should be equal to data block width
        block->IsValid = false;
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