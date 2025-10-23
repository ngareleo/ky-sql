#include <stdio.h>
#include <stdlib.h>
#include "Metadata/metadata.h"
#include "./include/translation_context.h"

// !! Here lies dragons !!
// !! This is a simple way to hoist global state. We must be careful.
// !! This could potentially chow memory esp during long sessions
// !! Will build something better in future
TranslationContext *gCxt = NULL;

TranslationContext *GetTranslationContext()
{
    if (!gCxt)
    {
        // ?? In future we might introduce more frequent clean up
        // ?? This is a long-lived struct
        // ?? It could grow and chowy memory during long sessions
        fprintf(stderr, "(get-translation-context) global context not initialized \n");
        return -1;
    }

    return gCxt;
}

void DisposeTranslationContext()
{
    if (gCxt)
    {
        FreeTranslationContext(gCxt);
        gCxt = NULL;
    }
}

void FreeTranslationContext(TranslationContext *ctx)
{
    if (!ctx)
    {
        FreeFileMetadata(ctx->FileMetadata);
        free(ctx);
    }
}

void FreeTranslationInitObj(TranslationInitObj *obj)
{
    if (obj)
    {
        fflush(obj->File);
        fclose(obj->File);
        free(obj);
    }
}

int InitTranslationContext(TranslationInitObj *initObj)
{
    if (gCxt)
    {
        fprintf(stderr, "(init-translation-context) global context already initialized \n");
        return -1;
    }

    do
    {
        gCxt = malloc(sizeof(TranslationContext));
        if (!gCxt)
        {
            fprintf(stderr, "(init-translation-context) malloc failed \n");
            break;
        }

        gCxt->FileMetadata = malloc(sizeof(FileMetadata));
        if (!gCxt->FileMetadata)
        {
            fprintf(stderr, "(init-translation-context) malloc failed \n");
            break;
        }

        if (ReadMetadataFromFile(initObj->File, gCxt->FileMetadata, CreateMetadataFromWritable) != 0)
        {
            fprintf(stderr, "(init-translation-context) couldn't read metadata from file \n");
            break;
        }

        fprintf(stderr, "(init-translation-context) translation context inialized \n");
        return;
    } while (0);

    free(gCxt->FileMetadata);
    free(gCxt);
    gCxt = NULL;
    return 0;
}
