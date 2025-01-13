#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "./utilities.c"

void test_count_tokens()
{
    printf(">> It should return -1 for empty strings");
    assert(counttokens("", ' ') == -1);
    printf("✅ \n");

    printf(">> It should count whitespaces as no tokens");
    assert(counttokens("       ", ' ') == 0);
    printf("✅ \n");

    printf(">> It should count one string properly");
    assert(counttokens("Hello", ' ') == 1);
    printf("✅ \n");

    printf(">> It should count one even with padding");
    assert(counttokens(" Hello  ", ' ') == 1);
    printf("✅ \n");

    printf(">> It should count two string properly");
    assert(counttokens("Hello world", ' ') == 2);
    printf("✅ \n");

    printf(">> It should count two strings properly even with padding");
    assert(counttokens("Hello    world   ", ' ') == 2);
    printf("✅ \n");

    printf(">> It should count multiple tokens properly");
    assert(counttokens("In a galaxy far far away", ' ') == 6);
    printf("✅ \n");

    printf(">> Test that it checks input for termination ");
    assert(tokenize("test") == NULL);
    printf("✅ \n");

    printf(">> Edge case. Multiple single characters");
    assert(counttokens("a b c d e f g", ' ') == 7);
    printf("✅ \n");
}

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
    printf("\n");
    test_count_tokens();
    printf("\nAll tests passed! \n");
    return 0;
}
