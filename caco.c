#include <stdio.h>
#include <stdlib.h>

typedef enum {
    VALUE, EQUAL, VAR, FUNC, ARG
} Types;

typedef struct {
    Types type;
    char *value;
} Token;

typedef struct {
    char *content;
    long length;
} String;

String read_file(char *filename) {
    FILE *file = fopen(filename, "r");
    String text;

    fseek(file, 0, SEEK_END);
    text.length = ftell(file);
    rewind(file);

    text.content = malloc(text.length * sizeof(char));
    fread(text.content, text.length, 1, file);
    fclose(file);

    return text;
}

String load(char *filename) {
    String text = read_file(filename);
    return text;
}

int main() {
    String text = load("script.caco");
    printf("%s", text.content);
}
