#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "utilities.c"

#define BUFFER_MAX 200
#define MAX_TOKENS 100
#define EXIT_MESSAGE "\nFarewell traveler!!\n"
#define QUIT "quit"

struct config
{
    char *target;
};

int make_config(struct config *config, int argc, char *argv[]);
int read_shell_turn(char *buffer, int max);
int parse_shell_input(char **tokens);
int parse_select_statement(char **tokens);

int main(int argc, char *argv[])
{
    char input[BUFFER_MAX],
        *pinput = input,
        **tokens;

    FILE *tstream;
    struct config conf, *pconfig = &conf;

    if (make_config(pconfig, argc, argv) < 0)
    {
        fprintf(stderr, "Usage for %s:\n", *argv);
        return -1;
    }

    if (pconfig->target == NULL && (tstream = fopen(pconfig->target, "r+")) == NULL)
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

        if (tokenize(pinput, tokens) < 0)
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

    for (int j = 0; tokens + j != NULL; j++)
        free(*(tokens + j));

    free(tokens);
    fclose(tstream);
    return 0;
}

int make_config(struct config *config, int argc, char *argv[])
{
    if (argc < 2)
        return -1;

    struct stat s;
    int ret = stat(*(argv + 1), &s);

    if (ret < 0)
    {
        if (errno == ENOENT)
        {
            // we setup a file
            printf("We setup a file at path %s\n", *(argv + 1));
            return -1;
        }
    }
    config->target = *(argv + 1);
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

int parse_shell_input(char **tokens)
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

int parse_select_statement(char **tokens)
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
