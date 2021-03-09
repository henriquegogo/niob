#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long length = 0;
long pos = 0;

typedef enum {
    NONE, STRING, VALUE, EQUAL, DEF, FUNC, ARG
} Types;

typedef struct Token {
    Types type;
    char *value;
} Token;

int is_space(char ch) {
    return ch == ' ' || ch == '\t';
}

int is_equal(char ch) {
    return ch == '=';
}

int is_newline(char ch) {
    return ch == '\n' || ch == '\r';
}

int is_alpha(char ch) {
    return ch >= 'A' && ch <= 'Z' \
        || ch >= 'a' && ch <= 'z' \
        || ch == '_';
}

int is_quote(char ch) {
    return ch == '"' || ch == '\'';
}

Token next_token(char *text) {
    while (is_space(text[pos])) ++pos;
    while (is_newline(text[pos])) ++pos;

    if (is_equal(text[pos])) {
        ++pos;
        Token token = { EQUAL, NULL };

        return token;
    }
    else if (is_alpha(text[pos])) {
        long initial_pos = pos;
        while (is_alpha(text[pos])) ++pos;
        long chars_size = pos - initial_pos;

        Token token = { DEF, malloc(chars_size * sizeof(char)) };
        memcpy(token.value, &text[initial_pos], chars_size);

        return token;
    }
    else if (is_quote(text[pos])) {
        char quote_char = text[pos];
        ++pos;
        long initial_pos = pos;
        while (text[pos] != quote_char) ++pos;
        long chars_size = pos - initial_pos;

        Token token = { STRING, malloc(chars_size * sizeof(char)) };
        memcpy(token.value, &text[initial_pos], chars_size);

        return token;
    }

    Token token = { NONE, NULL };

    return token;
}

Token* lexer(char *text) {
    Token *tokens;
    long i = 0;

    while (pos < length) {
        tokens = realloc(tokens, (i + 1) * sizeof(Token));
        Token token = next_token(text);
        
        if (token.type != NONE) {
            tokens[i] = token;
            ++i;
        }

        ++pos;
    }

    return tokens;
}

char *read_file(char *filename) {
    FILE *file = fopen(filename, "r");
    char *text;

    fseek(file, 0, SEEK_END);
    length = ftell(file);
    rewind(file);

    text = malloc(length * sizeof(char));
    fread(text, length, 1, file);
    fclose(file);

    return text;
}

Token *load(char *filename) {
    char *text = read_file(filename);
    Token *tokens = lexer(text);
    return tokens;
}

int main() {
    Token *tokens = load("script.caco");
    printf("%i (%s)\n", tokens[0].type, tokens[0].value);
    printf("%i (%s)\n", tokens[1].type, tokens[1].value);
    printf("%i (%s)\n", tokens[2].type, tokens[2].value);
    printf("%i (%s)\n", tokens[3].type, tokens[3].value);
    printf("%i (%s)\n", tokens[4].type, tokens[4].value);
    printf("%i (%s)\n", tokens[5].type, tokens[5].value);
}
