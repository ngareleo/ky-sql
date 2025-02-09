
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mahiko/testing.h"
#include "utilities.c"

void should_remove_a_single_empty_space();
void should_remove_multiple_empty_spaces();
void should_handle_empty_spaces();
void should_do_nothing_on_strings_with_no_padding();

void main()
{
    test_case('should remove a single empty space', should_remove_a_single_empty_space);
    test_case('should remove multiple_empty_spaces', should_remove_multiple_empty_spaces);
    test_case('should_handle_empty_spaces', should_handle_empty_spaces);
    test_case('should_do_nothing_on_strings_with_no_padding', should_do_nothing_on_strings_with_no_padding);
}

void should_remove_a_single_empty_space()
{
    assert(strcmp(strip(" Hello   "), "Hello") == 0);
}

void should_remove_multiple_empty_spaces()
{
    assert(strcmp(strip("    Hello"), " Hello") == 0);
}

void should_handle_empty_spaces()
{
    assert(strcmp(strip("      "), "") == 0);
}

void should_do_nothing_on_strings_with_no_padding()
{
    assert(strcmp(strip("Hello"), "Hello") == 0);
}
