
#ifndef DATA_BLOCK_TYPE_H
#define DATA_BLOCK_TYPE_H

/**
 * A data block is a row-based 2D ordered data type.
 *
 * Used to describe data for writes and to pack data from reads.
 */
typedef struct
{
    char ***Values; // The actual data
    int Validated;  // A valid data block has equal width across all rows
    struct          //
    {               //
        int Width;  // The number of columns
        int Count;  // The number of rows
    } Size;         // Dimensions of the data block
} DataBlockType;

DataBlockType *CreateDataBlock(char ***);
int ValidateDataBlock(DataBlockType *);
int SizeDataBlock(DataBlockType *);

#endif