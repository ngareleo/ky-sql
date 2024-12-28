#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_SIZE 200 // chars
#define QUIT "quit"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage for %s:\n", *argv);
        return -1;
    }


    printf("Ky-sql says hi!\n");

    char input[MAX_QUERY_SIZE], *targetfn = *argv + 1;
    FILE *tstream;

    tstream = fopen(targetfn, "r+");

    while (1) {
        char c, *pinput = input;
        printf("(kysql) : ");

        while ((c = getchar()) != EOF) {
            if (c == '\n') 
                break;
            *(pinput++) = c;
        }

        if (!strcmp(input, QUIT)){
            printf("\nAll the best traveler\n");
            break;
        }

        printf("%s\n", input);
        
        memset(input, 0, strlen(input));
    }

    return 0;
}