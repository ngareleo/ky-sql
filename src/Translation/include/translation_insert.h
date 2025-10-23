#ifndef TRANSLATION_INSERT_H
#define TRANSLATION_INSERT_H

#include "../Lang/language.h"
#include "../Writer/writer.h"

int LangInsertStmtToWriteRequest(Linsmt *, WriteRequest **);
int ValidateLanguageInsertStatement(Linsmt *);

#endif