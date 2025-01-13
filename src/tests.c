#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "./utilities.c"

void test_tokenize()
{

    printf(">> Test that it doesn't tokenize empty strings ");
    assert(tokenize("") == NULL);
    assert(tokenize("         ") == NULL);
    printf("✅ \n");

    printf(">> Test that it checks input for termination ");
    assert(tokenize("test") == NULL);
    printf("✅ \n");

    char *mock = "Hello;", **out;

    printf(">> Test that one word is tokenized correctly ");
    out = tokenize(mock);
    assert(strcmp(out[0], "Hello") == 0);
    multifree((void **)out, 1);
    printf("✅ \n");

    printf(">> Test that hello world is tokenized correctly");
    mock = "Hello World;";
    out = tokenize(mock);
    assert(strcmp(out[0], "Hello") == 0);
    assert(strcmp(out[1], "World") == 0);
    multifree((void **)out, 2);
    printf("✅ \n");
}

int main()
{

    printf("Running tests .............\n\n");
    test_tokenize();
    printf("\nAll tests passed! \n");
    return 0;
}
