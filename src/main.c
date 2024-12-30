#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_MAX 200 
#define QUIT "quit"

typedef struct {
    char *target;
} Config;

int makeConfig(Config *config, int argc, char *argv[]);
int readShellTurn(char *buffer);

int main(int argc, char *argv[]) {
    char input[BUFFER_MAX], *pinput = input;
    FILE *tstream;
    Config config, *pconfig = &config;

    if (makeConfig(pconfig, argc, argv) < 0) {
        printf("Usage for %s:\n", *argv);
        return -1;
    }
    
    if ((tstream = fopen(pconfig->target, "r+")) == NULL) {
        printf("Error opening stream\n");
        return -1;
    }

    while (1) {
        int in = readShellTurn(pinput);

        if (in == 0) {
            printf("Farewell traveler!!\n");
            break;
        } else if (in < 0) {
            printf("Error reading input. Try again.\n");
            break;
        } else {
            // for now we spit back
            printf("%s\n", pinput);        
        }

        memset(pinput, '\0', in);
    }

    fclose(tstream);
    return 0;
}

int makeConfig(Config *config, int argc, char *argv[]) {
    if (argc < 2) {
        return -1;
    }

    printf("Break >>\n");


    config->target = *(argv + 1);
    return 0;
} 


/**
 * Reads turn input from the shell. 
 * Returns the number of bytes written per turn, 0 if user quit and returns <0 on error
 */
int readShellTurn(char *buffer) {
    char c;
    int i;

    printf("\n(kysql) : ");

    while ((c = getchar()) != EOF) {
        if (c == '\n') 
            break;
        *(buffer + i) = c;
    }

    if (!strcmp(buffer, QUIT)){
        return 0;
    }

    return i;
}