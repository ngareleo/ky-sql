#include <stdio.h>
#include <stdlib.h>
#include "Metadata/metadata.h"
#include "./include/translation_context.h"

// !! Here lies dragons !!
// !! This is a simple way to hoist global state. We must be careful.
// !! This could potentially chow memory esp during long sessions
// !! Will build something better in future
TranslationCtx *gCxt = NULL;
FILE *readable = NULL;

TranslationCtx *GetTranslationContext()
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

void FreeTranslationContext(TranslationCtx *ctx)
{
    if (!ctx)
    {
        FreeFileMetadata(ctx->FileMd);
        free(ctx);
    }
}

void FreeTranslationInitObj(TranslationInitObj *obj)
{
    if (obj)
    {
        fflush(readable);
        fclose(readable);
        free(obj);
    }
}

int InitTranslationContext(TranslationInitObj *obj)
{
    if (gCxt)
    {
        fprintf(stderr, "(init-translation-context) global context already initialized \n");
        return -1;
    }

    if (readable)
    {
        fflush(readable);
        clearerr(readable);
    }
    else
    {
        readable = fopen(obj->File, "r");
    }

    do
    {
        gCxt = malloc(sizeof(TranslationCtx));
        if (!gCxt)
        {
            fprintf(stderr, "(init-translation-context) malloc failed \n");
            break;
        }

        gCxt->FileMd = malloc(sizeof(FileMetadata));
        if (!gCxt->FileMd)
        {
            fprintf(stderr, "(init-translation-context) malloc failed \n");
            break;
        }

        if (ReadMetadataFromFile(readable, gCxt->FileMd, CreateMetadataFromWritable) != 0)
        {
            fprintf(stderr, "(init-translation-context) couldn't read metadata from file \n");
            break;
        }

        fprintf(stderr, "(init-translation-context) translation context inialized \n");
        return 0;
    } while (0);

    // If the initalization block fails, we reset and cleanup everything
    free(gCxt->FileMd);
    free(gCxt);
    gCxt = NULL;
    return -1;
}
