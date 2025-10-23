
#ifndef TRANSLATION_CONTEXT_H
#define TRANSLATION_CONTEXT_H

#include <stdio.h>
#include "../../Metadata/metadata.h"

typedef struct
{
    FileMetadata *FileMd;
} TranslationCtx;

typedef struct
{
    char *File;
} TranslationInitObj;

TranslationCtx *GetTranslationContext();
void FreeTranslationContext(TranslationCtx *);
int InitTranslationContext(TranslationInitObj *);
void DisposeTranslationContext();

#endif