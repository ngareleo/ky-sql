#ifndef TRANSLATION_QUERY_H
#define TRANSLATION_QUERY_H

#include "../Lang/language.h"
#include "../Reader/reader.h"

int LangQueryToReadRequest(LanguageQueryStatement *, ReadRequest **);

#endif