#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.c"
#include "config.c"

#define BUFFER_MAX 200
#define MAX_TOKENS 100
#define EXIT_MESSAGE "\nFarewell traveler!!\n"
#define QUIT "quit"

int read_shell_turn(char *buffer, int max);
int parse_shell_input(Tokens tokens);
int parse_select_statement(Tokens tokens);

int main(int argc, char *argv[])
{
    char input[BUFFER_MAX],
        *pinput = input;
    Tokens tokens;
    FILE *tstream;
    Config conf, *pconfig = &conf;

    if (!make_config(pconfig, argc, argv))
    {
        fprintf(stderr, "Usage for %s:\n", *argv);
        return -1;
    }

    if ((tstream = fopen(pconfig->target, "r+")) == NULL)
    {
        fprintf(stderr, "Error opening stream\n");
        return -1;
    }

    while (1)
    {
        int in, parse;

        if ((in = read_shell_turn(pinput, BUFFER_MAX)) < 0)
        {
            fprintf(stderr, "Error reading input. Try again.\n");
            break;
        }

        if ((tokens = tokenize(pinput)) == NULL)
        {
            fprintf(stderr, "Couldn't tokenize input text\n");
            break;
        }

        parse = parse_shell_input(tokens);

        if (parse < 0)
        {
            fprintf(stderr, "Trouble parsing input\n");
            break;
        }
        else if (parse == 0)
        {
            printf(EXIT_MESSAGE);
            break;
        }

        memset(pinput, '\0', in);
    }

    // cleanups

    int tokencount = count_tokens_flat_list(tokens);
    if (tokencount > 0)
        free_tokens(tokens, tokencount);

    fclose(tstream);
    return 0;
}

int read_shell_turn(char *buffer, int max)
{
    char c;
    int i = 0;

    printf("\n(kysql) : ");

    while ((c = getchar()) != EOF && i < max)
    {
        if (c == '\n')
        {
            printf("       -> ");
            *(buffer + i++) = ' ';
        }
        else if (c == ';')
            break;
        else
            *(buffer + i++) = c;
    }

    return i;
}

int parse_shell_input(Tokens tokens)
{
    if (!strcmp(*tokens, QUIT))
        return 0;

    if (parse_select_statement(tokens) < 0)
    {
        fprintf(stderr, "Not select statement\n");
        return -1;
    }

    return -1;
}

int parse_select_statement(Tokens tokens)
{
    char *t_select = "select",
         *ut_select = "SELECT",
         *t_from = "from",
         *ut_from = "FROM";

    printf("Value of 0 --> %s", *tokens);

    if (strcmp(*tokens, t_select) || strcmp(*tokens, ut_select))
    {
        return -1;
    }

    return 0;
}
