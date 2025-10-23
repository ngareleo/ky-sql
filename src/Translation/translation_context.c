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
// !! Will build a better abstraction in future
TranslationContext *globalCxt = NULL;

TranslationContext *GetTranslationContext()
{
    if (!globalCxt)
    {
        // ?? In future we might introduce more frequent clean up
        // ?? This is a long-lived struct
        // ?? It could grow and chowy memory during long sessions
        fprintf(stderr, "(get-translation-context) global context not initialized \n");
        return -1;
    }

    return globalCxt;
}

typedef struct
{
    FILE *File;
} TranslationInitObj;

int *InitTranslationContext(TranslationInitObj *initObj)
{
    if (globalCxt)
    {
        fprintf(stderr, "(init-translation-context) global context already initialized \n");
        return -1;
    }

    do
    {
        globalCxt = malloc(sizeof(TranslationContext));
        if (!globalCxt)
        {
            fprintf(stderr, "(init-translation-context) malloc failed \n");
            break;
        }

        // Initialize metadata from file
        globalCxt->FileMetadataObj = malloc(sizeof(FileMetadata));
        if (!globalCxt->FileMetadataObj)
        {
            fprintf(stderr, "(init-translation-context) malloc failed \n");
            break;
        }

        if (ReadMetadataFromFile(initObj->File, globalCxt->FileMetadataObj, CreateMetadataFromWritable) != 0)
        {
            fprintf(stderr, "(init-translation-context) couldn't read metadata from file \n");
            break;
        }

        // Success
        fprintf(stderr, "(init-translation-context) translation context inialized \n");
        return;
    } while (0);

    // Catch block
    free(globalCxt->FileMetadataObj);
    free(globalCxt);

    if (!globalCxt)
    {
        globalCxt = NULL;
    }

    return 0;
}
