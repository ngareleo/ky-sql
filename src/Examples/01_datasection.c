#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "Metadata/metadata.h"
#include "Translation/translation.h"
#include "Mocks/mocks.h"

int main()
{
    char *file = "noop.kysql", *readBuff;
    FILE *writable = fopen(file, "w");

    WriteRequest *writeReq;
    ReadRequest *readReq;
    TranslationInitObj *obj;
    Linsmt *booksLinmt;
    Liqsmt *booksLiqsmt;
    WriterMetadata *writer;
    ReaderMetadata *reader;

    obj->File = file;
    writer->Writable = writable;
    reader->File = file;
    reader->ReadBuffer = readBuff;

    InitTranslationContext(obj);
    booksLinmt = MockBookTableInsertStatement();

    do
    {
        if (!booksLinmt)
        {
            fprintf(stderr, "(01_datasection) failed to create insert statement \n");
            break;
        }

        if (LinsmtToWriteRequest(booksLinmt, &writeReq) != 0)
        {
            fprintf(stderr, "(01_datasection) failed to translate insert statement to write request \n");
            break;
        }

        if (HandleWrite(writeReq, writer) != 0)
        {
            fprintf(stderr, "(01_datasection) write to file failed \n");
            break;
        }

        booksLiqsmt = CreateMockBooksLiqsmt();
        if (!booksLiqsmt)
        {
            fprintf(stderr, "(01_datasection) failed to create language query statement \n");
            break;
        }

        if (LiqsmtToReadRequest(booksLiqsmt, &readReq) != 0)
        {
            fprintf(stderr, "(01_datasection) failed to create language query statement \n");
            break;
        }

        if (HandleRead(readReq, reader) != 0)
        {
            fprintf(stderr, "(01_datasection) failed to create language query statement \n");
            break;
        }

        fprintf(stderr, "(01_datasection) Data retreieved successfully \n");

    } while (0);

    FreeTranslationInitObj(obj);
    FreeLinsmt(booksLinmt);
    FreeWriteRequest(writeReq);
    DisposeTranslationContext();
    FreeWriteMetadata(writer);
    fclose(writable);
}