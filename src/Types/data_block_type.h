#ifndef DATA_BLOCK_TYPE_H
#define DATA_BLOCK_TYPE_H

#include <stdbool.h>

typedef struct
{
    int Width;       // The number of columns
    int Count;       // The number of rows
    int HeaderCount; // The number of columns in headers
} DataBlockSize;

/**
 * A data block is a row-based 2D ordered data type
 * Used to describe data for writes and to pack data from reads
 */
typedef struct
{
    char **Header;       // The header of the structure
    char ***Values;      // The actual data
    bool IsAligned;      // Indicates whether each row has same number of elements
    bool IsEmpty;        // Indicates whether the structure is missing data values
    bool IsHeaderLess;   // Indicates whether the structure has not headers
    bool IsValid;        // A valid data block is okay to deal with and won't cause issues. An example is if the number of headers is more than rows
    DataBlockSize *Size; // Dimensions of the data block
} DataBlock;

DataBlock *CreateDataBlock(const char **, const char ***);
void ValidateDataBlock(DataBlock *);
DataBlockSize *MeasureBlockStructure(const char **, const char ***);
DataBlockSize *ZeroBlockSize();
DataBlock *EmptyDataBlock();
void FreeDataBlock(DataBlock *);

#endif