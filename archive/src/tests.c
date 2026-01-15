// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include "./utilities.c"

// void execute_suite(char *suite, void(func)());

// static void sig_handler(int signo)
// {
//     if (signo == SIGABRT)
//     {
//         fflush(NULL);
//     }
// }

// void test_strip()
// {
//     printf(">> It should remove a single empty space");
//     char *in = " Hello";
//     assert(strcmp(strip(in), "Hello") == 0);
//     printf(" ✅ \n");

//     printf(">> It should remove multiple empty spaces with text");
//     in = "     Hello";
//     assert(strcmp(strip(in), "Hello") == 0);
//     printf(" ✅ \n");

//     printf(">> It should remove multiple empty spaces with no text");
//     in = "                 ";
//     assert(strcmp(strip(in), "") == 0);
//     printf(" ✅ \n");

//     printf(">> It should do nothing on strings with no padding");
//     in = "Hello";
//     assert(strcmp(strip(in), "Hello") == 0);
//     printf(" ✅ \n");
// }

// void test_count_tokens_from_text()
// {
//     printf(">> It should return -1 for empty strings");
//     assert(count_tokens_from_text("", ' ') == -1);
//     printf(" ✅ \n");

//     printf(">> It should return -1 for strings with multiple whitespaces");
//     assert(count_tokens_from_text("       ", ' ') == -1);
//     printf(" ✅ \n");

//     printf(">> It should count one token properly");
//     assert(count_tokens_from_text("Hello", ' ') == 1);
//     printf(" ✅ \n");

//     printf(">> It should count one token properly even with padding");
//     assert(count_tokens_from_text(" Hello  ", ' ') == 1);
//     printf(" ✅ \n");

//     printf(">> It should count two tokens properly");
//     assert(count_tokens_from_text("Hello world", ' ') == 2);
//     printf(" ✅ \n");

//     printf(">> It should count two tokens properly even with padding");
//     assert(count_tokens_from_text("Hello    world   ", ' ') == 2);
//     printf(" ✅ \n");

//     printf(">> It should count multiple tokens properly");
//     assert(count_tokens_from_text("In a galaxy far far away", ' ') == 6);
//     printf(" ✅ \n");

//     printf(">> (edge case) Multiple single characters");
//     assert(count_tokens_from_text("a b c d e f g", ' ') == 7);
//     printf(" ✅ \n");
// }

// void test_tokenize()
// {
//     printf(">> Test that it doesn't tokenize empty strings ");
//     assert(tokenize("") == NULL);
//     assert(tokenize("         ") == NULL);
//     printf("✅ \n");

//     printf(">> Test that word is not tokenized without trailing semicolon");

//     assert(tokenize("Hello World") == NULL);
//     printf("✅ \n");

//     char *mock = "Hello;", **out;
//     printf(">> Test that hello world is tokenized correctly");
//     mock = "Hello World;";
//     out = tokenize(mock);
//     assert(strcmp(out[0], "Hello") == 0);
//     assert(strcmp(out[1], "World") == 0);
//     free_tokens(out);
//     printf("✅ \n");

//     printf(">> Test that long random string is tokenized correctly");
//     mock = "Hello      World To my People;";
//     out = tokenize(mock);
//     assert(strcmp(out[0], "Hello") == 0);
//     assert(strcmp(out[1], "World") == 0);
//     assert(strcmp(out[2], "To") == 0);
//     assert(strcmp(out[3], "my") == 0);
//     assert(strcmp(out[4], "People") == 0);
//     free_tokens(out);
//     printf("✅ \n");

//     printf(">> Should work properly with non-alphanumerics and single characters");
//     mock = "a f gs gs * fs    fs ss s @;";
//     out = tokenize(mock);
//     assert(strcmp(out[0], "a") == 0);
//     assert(strcmp(out[1], "f") == 0);
//     assert(strcmp(out[2], "gs") == 0);
//     assert(strcmp(out[3], "gs") == 0);
//     assert(strcmp(out[4], "*") == 0);
//     assert(strcmp(out[5], "fs") == 0);
//     assert(strcmp(out[6], "fs") == 0);
//     assert(strcmp(out[7], "ss") == 0);
//     assert(strcmp(out[8], "s") == 0);
//     assert(strcmp(out[9], "@") == 0);
//     free_tokens(out);
//     printf("✅ \n");
// }

// void test_count_tokens_from_single_list()
// {

//     printf(">> Test that it doesn't count null values ");
//     assert(count_tokens_flat_list(NULL) == -1);
//     printf("✅ \n");

//     char **mock = (char **)malloc(sizeof(char *) * 2);
//     mock[0] = "Hello";
//     mock[1] = "World";

//     printf(">> Test that it counts properly");
//     assert(count_tokens_flat_list(mock) == 2);
//     printf("✅ \n");
// }

// void test_capitalize()
// {
//     printf(">> Test it doesn't capitalize NULL");
//     assert(upper(NULL) == NULL);
//     printf("✅ \n");

//     printf(">> Test it capitalizes properly");
//     char *mock = "Hello World", *out;
//     out = upper(mock);
//     assert(strcmp(out, "HELLO WORLD") == 0);
//     printf("✅ \n");
//     free(out);
// }

// int main()
// {
//     if (signal(SIGABRT, sig_handler) == SIG_ERR)
//     {
//         fprintf(stderr, "Cannot handle signal after assert fail");
//         exit(EXIT_FAILURE);
//     }

//     printf("Running tests .............\n\n");
//     execute_suite("Test count tokens from list", test_count_tokens_from_single_list);
//     execute_suite("Test strip", test_strip);
//     execute_suite("Test count tokens from text", test_count_tokens_from_text);
//     execute_suite("Test tokenize", test_tokenize);
//     execute_suite("Test capitalize", test_capitalize);

//     return 0;
// }

// void execute_suite(char *suite, void(func)())
// {
//     printf("Executing test suite: %s\n\n", suite);
//     func();
//     printf("success\n\n");
// }