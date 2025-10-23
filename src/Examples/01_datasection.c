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
    FILE *readable = fopen(file, "r");
    FILE *writable = fopen(file, "w");

    WriteRequest *writeReq;
    TranslationInitObj *obj;
    Linsmt *bookTableIns;
    WriterMetadata *writer;

    obj->File = readable;
    writer->Writable = writable;

    InitTranslationContext(obj);
    bookTableIns = MockBookTableInsertStatement();

    if (!bookTableIns)
    {
        fprintf(stderr, "(01_datasection) failed to create insert statement \n");
        return -1;
    }

    if (LinsmtToWriteRequest(bookTableIns, &writeReq) != 0)
    {
        fprintf(stderr, "(01_datasection) failed to translate insert statement to write request \n");
        return -1;
    }

    if (HandleWrite(writeReq, writer) != 0)
    {
        fprintf(stderr, "(01_datasection) write to file failed \n");
        return -1;
    }
}