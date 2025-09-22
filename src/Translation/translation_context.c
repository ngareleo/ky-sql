#include <stdio.h>
#include <stdlib.h>
#include "../Metadata/metadata.h"
#include "include/translation_context.h"

typedef struct
{
    FileMetadata *FileMetadataObj;
} TranslationContext;

// !!
// !! Here lies dragons
// !! A simple way to hoist global state
// !! We must be careful.
// !! This could potentially chow memory esp in long sessions
// !!
// !! As well as a huge source of memory leaks
// !! A good place to statement when we start optimizing
// !!
TranslationContext *globalContext = NULL;

TranslationContext *GetTranslationContext()
{
    if (!globalContext)
    {
        // ?? In future we might introduce clean up
        // ?? This is a long-lived struct
        // ?? It could grow and chowy memory during long sessions
        fprintf(stderr, "(get-translation-context) global context not initialized \n");
        return -1;
    }

    return globalContext;
}

typedef struct
{
    FILE *File;
} TranslationInitObj;

int *InitTranslationContext(TranslationInitObj *initObj)
{
    if (globalContext)
    {
        fprintf(stderr, "(init-translation-context) global context already initialized \n");
        return -1;
    }

    globalContext = malloc(sizeof(TranslationContext));
    if (!globalContext)
    {
        fprintf(stderr, "(init-translation-context) malloc failed \n");
        return -1;
    }

    globalContext->FileMetadataObj = malloc(sizeof(FileMetadata));
    if (!globalContext->FileMetadataObj)
    {
        free(globalContext);
        fprintf(stderr, "(init-translation-context) malloc failed \n");
        return -1;
    }

    FileMetadata *fmPtr = globalContext->FileMetadataObj;
    // Here we read the metadata section of the file
    if (ReadMetadataFromFile(initObj->File, &fmPtr, CreateMetadataFromWritable) != 0)
    {
        free(globalContext->FileMetadataObj);
        free(globalContext);
        fprintf(stderr, "(init-translation-context) couldn't read metadata from file \n");
        return -1;
    }

    fprintf(stderr, "(init-translation-context) translation context inialized \n");
    return 0;
}
