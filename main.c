#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "niob.h"

int main(int argc, char **argv) {
    niob_init();

    if (argc == 1) {
        char input[1024];
        while (strcmp(input, "exit\n") != 0) {
            printf("niob> ");
            fflush(stdout);
            fgets(input,1024,stdin);
            niob_eval(input);
        }
    } else if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        rewind(file);
        char *text = malloc(size);
        fread(text, size, 1, file);
        niob_eval(text);
        fclose(file);
    }

    return 0;
}
