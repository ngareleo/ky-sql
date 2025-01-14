#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "./utilities.c"

void execute_suite(char *suite, void(func)());

void test_strip()
{
    printf(">> It should remove single empty space");
    char *in = " Hello";
    strip(in);
    assert(strcmp(in, "Hello"));
    printf(" ✅ \n");

    printf(">> It should remove multiple empty spaces with text");
    in = "     Hello";
    strip(in);
    assert(strcmp(in, "Hello"));
    printf(" ✅ \n");

    printf(">> It should remove multiple empty spaces with no text");
    in = "                 ";
    strip(in);
    assert(strcmp(in, ""));
    printf(" ✅ \n");

    printf(">> It should do nothing on strings with no padding");
    in = "Hello";
    strip(in);
    assert(strcmp(in, "Hello"));
    printf(" ✅ \n");
}

void test_count_tokens_from_text()
{
    printf(">> It should return -1 for empty strings");
    assert(count_tokens_from_text("", ' ') == -1);
    printf(" ✅ \n");

    printf(">> It should count multiple whitespaces as 0 tokens");
    assert(count_tokens_from_text("       ", ' ') == -1);
    printf(" ✅ \n");

    printf(">> It should count one string properly");
    assert(count_tokens_from_text("Hello", ' ') == 1);
    printf(" ✅ \n");

    printf(">> It should count one even with padding");
    assert(count_tokens_from_text(" Hello  ", ' ') == 1);
    printf(" ✅ \n");

    printf(">> It should count two string properly");
    assert(count_tokens_from_text("Hello world", ' ') == 2);
    printf(" ✅ \n");

    printf(">> It should count two strings properly even with padding");
    assert(count_tokens_from_text("Hello    world   ", ' ') == 2);
    printf(" ✅ \n");

    printf(">> It should count multiple tokens properly");
    assert(count_tokens_from_text("In a galaxy far far away", ' ') == 6);
    printf(" ✅ \n");

    printf(">> Test that it checks input for termination");
    assert(tokenize("test") == NULL);
    printf(" ✅ \n");

    printf(">> Edge case. Multiple single characters");
    assert(count_tokens_from_text("a b c d e f g", ' ') == 7);
    printf(" ✅ \n");
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
    free_tokens(out, 1);
    printf("✅ \n");

    printf(">> Test that hello world is tokenized correctly");
    mock = "Hello World;";
    out = tokenize(mock);
    assert(strcmp(out[0], "Hello") == 0);
    assert(strcmp(out[1], "World") == 0);
    free_tokens(out, 2);
    printf("✅ \n");
}

int main()
{
    printf("Running tests .............\n\n");
    execute_suite("Test strip", test_strip);
    execute_suite("Test count tokens from text", test_count_tokens_from_text);
    execute_suite("Test tokenize", test_tokenize);
    return 0;
}

void execute_suite(char *suite, void(func)())
{
    printf("Executing test suite: %s\n\n", suite);
    func();
    printf("success\n\n");
}