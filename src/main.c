#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_SIZE 200 // chars
#define QUIT "quit"

int main() {
    printf("Ky-sql says hi!\n");

    while (1) {
        char input[MAX_QUERY_SIZE], *pinput = input, c; 

        printf("(kysql) : ");

        while ((c = getchar()) != EOF) {
            if (c == '\n') 
                break;
            *(pinput++) = c;
        }

        if (!strcmp(pinput, QUIT)){
            printf("\nAll the best traveler\n");
            break;
        }

        printf("%s\n", input);
        
        memset(pinput, '\0', strlen(input) + 1);
    }

    return 0;
}