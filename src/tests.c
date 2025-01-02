#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "./utilities.c"

void test_tokenize()
{
    char *mock = "Hello", **out;

    // Test that one word is tokenized correctly
    tokenize(mock, out);
    assert(strcmp(*out, "Hello") == 0);

    // Test that hello world is tokenized correctly
    mock = "Hello World";
    out = NULL;
    tokenize(mock, out);

    assert(strcmp(*out, "Hello") == 0);
    assert(strcmp(*(out + 1), "World") == 0);

    out = NULL;
    // Test that it doesn't tokenize empty strings
    assert(tokenize("", out) == -1);
    assert(tokenize("   ", out) == -1);
}

int main()
{
    printf("Running tests .............\n\n");
    test_tokenize();

    return 0;
}
