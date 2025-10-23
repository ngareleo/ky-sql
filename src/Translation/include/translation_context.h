
#ifndef TRANSLATION_CONTEXT_H
#define TRANSLATION_CONTEXT_H

#include <stdio.h>
#include "../../Metadata/metadata.h"

typedef struct
{
    FileMetadata *FileMetadata;
} TranslationContext;

typedef struct
{
    FILE *File;
} TranslationInitObj;

TranslationContext *GetTranslationContext();
void FreeTranslationContext(TranslationContext *);
int InitTranslationContext(TranslationInitObj *);
void DisposeTranslationContext();

#endif