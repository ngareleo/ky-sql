#ifndef DATA_BLOCK_TYPE_H
#define DATA_BLOCK_TYPE_H

typedef struct
{              //
    int Width; // The number of columns
    int Count; // The number of rows
} DataBlockSize;

/**
 * A data block is a row-based 2D ordered data type.
 *
 * Used to describe data for writes and to pack data from reads.
 */
typedef struct
{
    char **Header;       // The header of the structure
    char ***Values;      // The actual data
    int IsAligned;       // Indicates whether each row has same number of elements
    int IsEmpty;         // Indicates whether the structure is missing data values
    int IsHeaderLess;    // Indicates whether the structure has not headers
    int IsValid;         // A valid data block is okay to deal with and won't cause issues. An example is if the number of headers is more than rows
    DataBlockSize *Size; // Dimensions of the data block

} DataBlockType;

DataBlockType *CreateDataBlock(char **, char ***);
void ValidateDataBlock(DataBlockType *);
int SizeDataBlock(DataBlockType *);

#endif