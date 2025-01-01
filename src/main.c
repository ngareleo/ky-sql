#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "./dtypes.h"

#define BUFFER_MAX 200 
#define EXIT_MESSAGE "\nFarewell traveler!!\n"
#define QUIT "quit"


typedef struct {
    char *target;
} Config;

int make_config(Config *config, struct stat *s, int argc, char *argv[]);
int read_shell_turn(char *buffer, int max);
int parse_table_defs(TableDef *def, Config *config);
int parse_shell_input(char *in);

int main(int argc, char *argv[]) {
    char input[BUFFER_MAX], *pinput = input;
    FILE *tstream;
    Config config, *pconfig = &config;
    struct stat sb, *psb = &sb;

    if (make_config(pconfig, psb, argc, argv) < 0) {
        printf("Usage for %s:\n", *argv);
        return -1;
    }
    
    if (pconfig->target == NULL && (tstream = fopen(pconfig->target, "r+")) == NULL) {
        printf("Error opening stream\n");
        return -1;
    }

    while (1) {
        int in = read_shell_turn(pinput, BUFFER_MAX), parse;

        if (in < 0) {
            printf("Error reading input. Try again.\n");
            break;
        }

        parse = parse_shell_input(pinput);
        if ( parse < 0) {
            printf("Trouble parsing input\n");
            break;
        } else if (parse == 0) {
            printf(EXIT_MESSAGE);
            break;
        } 

        memset(pinput, '\0', in);
    }

    fclose(tstream);
    return 0;
}

int make_config(Config *config, struct stat *s, int argc, char *argv[]) {
    if (argc < 2) {
        return -1;
    }

    int ret = stat(*(argv + 1), s);
    if (ret < 0) {
        if (errno == ENOENT) {
            // we setup a file
            printf("We setup a file at path %s\n", *(argv + 1));
            return -1;
        }
    }
    config->target = *(argv + 1);
    return 0;
} 

int read_shell_turn(char *buffer, int max) {
    char c;
    int i = 0;

    printf("\n(kysql) : ");

    while ((c = getchar()) != EOF && i < max) {
        if (c == '\n') 
            printf("       -> ");
        else if (c == ';') 
            break;
        *(buffer + i++) = c;
    }

    return i;
}

int parse_shell_input(char *in) {
    if (!strcmp(in, QUIT)){
        return 0;
    }
    return -1;
}