#ifndef TRANSLATION_INSERT_H
#define TRANSLATION_INSERT_H

#include "../Lang/language.h"
#include "../Writer/writer.h"

#define MAX_STRING_SIZE 1000000

int LinsmtToWriteRequest(Linsmt *, WriteRequest **);
int ValidateLanguageInsertStatement(Linsmt *);
TableColDefinition *MatchTableColFromLinsmt(char *, TableDefinition *);
TableDefinition *MatchTableDefFromLinsmt(Linsmt *, SchemaDefinition *);
WriteRequest *CreateWriteRequest(Linsmt *, TranslationCtx *);
char *TranslateLinsmt(Linsmt *, TranslationCtx *);
int SchemaValidateLinsmt(Linsmt *, TranslationCtx *);
int ValidateLinsmt(Linsmt *);

#endif