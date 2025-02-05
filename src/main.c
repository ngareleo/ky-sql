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
    int shell, parse;
    char input[BUFFER_MAX], *pinput = input, *upred_input, **tokens;
    FILE *tstream;
    Config conf, *pconfig = &conf;

    if (make_config(pconfig, argc, argv))
    {
        error("Usage for %s:\n", *argv);
        return -1;
    }

    if ((tstream = fopen(pconfig->target, "r+")) == NULL)
    {
        error("Error opening stream");
        return -1;
    }

    while (1)
    {
        shell = read_shell_turn(pinput, BUFFER_MAX);
        if (shell)
        {
            error("Error reading input. Try again.");
            break;
        }

        upred_input = upper(pinput);
        tokens = tokenize(upred_input);
        pr_tokens(tokens);

        if (tokens == NULL)
        {
            error("Couldn't tokenize input text");
            break;
        }

        parse = parse_shell_input(tokens);
        if (parse < 0)
        {
            error("Trouble parsing input");
            break;
        }
        else if (parse == 0)
        {
            info(EXIT_MESSAGE);
            break;
        }

        free(upred_input);
        memset(pinput, '\0', shell);
    }

    // cleanups

    int tokencount = count_tokens_flat_list(tokens);
    if (tokencount > 0)
        free_tokens(tokens);

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
        {
            *(buffer + i++) = ';';
            break;
        }
        else
            *(buffer + i++) = c;
    }
    *(buffer + i) = '\0';
    return i;
}

int parse_shell_input(Tokens tokens)
{
    if (!strcmp(*tokens, QUIT))
        return 0;

    if (parse_select_statement(tokens) < 0)
    {
        info("Not select statement");
        return -1;
    }

    return -1;
}

int parse_select_statement(Tokens tokens)
{
    // char *t_select = "select",
    //      *ut_select = "SELECT",
    //      *t_from = "from",
    //      *ut_from = "FROM";

    return 0;
}
