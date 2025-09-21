#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "../Lang/language.h"
#include "../Reader/reader.h"
#include "../Writer/writer.h"

int LangInsertToWriteRequest(LanguageInsertStatement *, WriteRequest **);
int LangQueryToReadRequest(LanguageQueryStatement *, ReadRequest **);

#endif