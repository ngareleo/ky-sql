#ifndef MOCKS_H
#define MOCKS_H

#include "../Metadata/metadata.h"
#include "../Lang/language.h"

// Mock metadata
TableDefinition *CreateCustomersTableDefinition();
TableDefinition *CreateBooksTableDefintion();
SchemaDefinition *CreateBookStoreSchemaDefinition();

// Mock data statements
LanguageInsertStatement *MockBookTableInsertStatement();
#endif