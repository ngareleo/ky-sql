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
    char *file = "noop.kysql";
    FILE *writable = fopen(file, "w");

    WriteRequest *writeReq;
    TranslationInitObj *obj;
    Linsmt *booksLinmt;
    WriterMetadata *writer;

    obj->File = "noop.kysql";
    writer->Writable = writable;

    InitTranslationContext(obj);
    booksLinmt = MockBookTableInsertStatement();

    if (!booksLinmt)
    {
        fprintf(stderr, "(01_datasection) failed to create insert statement \n");
        return -1;
    }

    if (LinsmtToWriteRequest(booksLinmt, &writeReq) != 0)
    {
        fprintf(stderr, "(01_datasection) failed to translate insert statement to write request \n");
        return -1;
    }

    if (HandleWrite(writeReq, writer) != 0)
    {
        fprintf(stderr, "(01_datasection) write to file failed \n");
        return -1;
    }

    FreeTranslationInitObj(obj);
    FreeLinsmt(booksLinmt);
    FreeWriteRequest(writeReq);
    DisposeTranslationContext();
    FreeWriteMetadata(writer);
    fclose(writable);
}