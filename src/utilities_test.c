
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mahiko/headers/testing.h"
#include "utilities.c"

void strip_should_remove_a_single_empty_space()
{
    assert(strcmp(strip(" Hello   "), "Hello") == 0);
}

void strip_should_remove_multiple_empty_spaces()
{
    assert(strcmp(strip("    Hello"), " Hello") == 0);
}

void strip_should_handle_empty_spaces()
{
    assert(strcmp(strip("      "), "") == 0);
}

void strip_should_do_nothing_on_strings_with_no_padding()
{
    assert(strcmp(strip("Hello"), "Hello") == 0);
}

void count_tokens_should_return_neg_for_empty_strings()
{
    assert(count_tokens_from_text("", ' ') == -1);
}

void count_tokens_should_count_single_token_properly()
{
    assert(count_tokens_from_text("       ", ' ') == -1);
}

void count_tokens_should_count_single_token_with_padding()
{
    assert(count_tokens_from_text(" Hello  ", ' ') == 1);
}

void count_tokens_should_count_multiple_tokens()
{
    assert(count_tokens_from_text("In a galaxy far far away", ' ') == 6);
}

void count_tokens_should_count_multiple_single_characters()
{
    assert(count_tokens_from_text("a b c d e f g", ' ') == 7);
}

void tokenize_should_n_tokenize_empty_strings()
{
    assert(tokenize("") == NULL);
    assert(tokenize("         ") == NULL);
}

void tokenize_should_tokenize_text_with_no_padding()
{
    assert(tokenize("Hello World") == NULL);
}

void tokenize_should_tokenize_strings_correctly()
{
    char *mock, **out;
    mock = "Hello World;";
    out = tokenize(mock);
    assert(strcmp(out[0], "Hello") == 0);
    assert(strcmp(out[1], "World") == 0);
    free_tokens(out);
}

void tokenize_should_tokenize_random_unevenly_spaced_strings_properly()
{
    char *mock, **out;
    mock = "Hello      World To my People;";
    out = tokenize(mock);
    assert(strcmp(out[0], "Hello") == 0);
    assert(strcmp(out[1], "World") == 0);
    assert(strcmp(out[2], "To") == 0);
    assert(strcmp(out[3], "my") == 0);
    assert(strcmp(out[4], "People") == 0);
    free_tokens(out);
}

void tokenize_should_tokenize_non_alphanumeric_spaced_characters()
{
    char *mock, **out;
    mock = "a f gs gs * fs    fs ss s @;";
    out = tokenize(mock);
    assert(strcmp(out[0], "a") == 0);
    assert(strcmp(out[1], "f") == 0);
    assert(strcmp(out[2], "gs") == 0);
    assert(strcmp(out[3], "gs") == 0);
    assert(strcmp(out[4], "*") == 0);
    assert(strcmp(out[5], "fs") == 0);
    assert(strcmp(out[6], "fs") == 0);
    assert(strcmp(out[7], "ss") == 0);
    assert(strcmp(out[8], "s") == 0);
    assert(strcmp(out[9], "@") == 0);
    free_tokens(out);
}

void count_tokens_should_flag_nulls()
{
    assert(count_tokens_flat_list(NULL) == -1);
}

void count_tokens_should_count_tokens()
{

    char **mock = (char **)malloc(sizeof(char *) * 2);
    mock[0] = "Hello";
    mock[1] = "World";
    assert(count_tokens_flat_list(mock) == 2);
    free(mock);
}

void upper_should_flag_nulls()
{
    assert(upper(NULL) == NULL);
}

void upper_should_uppercase()
{
    char *mock = "Hello World", *out;
    out = upper(mock);
    assert(strcmp(out, "HELLO WORLD") == 0);
    free(out);
}