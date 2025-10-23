#ifndef TRANSLATION_INSERT_H
#define TRANSLATION_INSERT_H

#include "../Lang/language.h"
#include "../Writer/writer.h"

int LinsmtToWriteRequest(Linsmt *, WriteRequest **);
int ValidateLanguageInsertStatement(Linsmt *);
TableColDefinition *MatchTableColFromLinsmt(char *, TableDefinition *);
TableDefinition *MatchTableDefFromLinsmt(Linsmt *, SchemaDefinition *);
WriteRequest *CreateWriteRequest(Linsmt *, TranslationContext *);
char *TranslateLinsmt(Linsmt *, TranslationContext *);
int SchemaValidateLinsmt(Linsmt *, TranslationContext *);
int ValidateLinsmt(Linsmt *);

#endif