#include <stdio.h>
#include <stdlib.h>

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
    return ch == '"';
}

int is_singlequote(char ch) {
    return ch == '\'';
}

Token next_token(char *text) {
    long i = 0;

    if (is_space(text[pos])) {
        while (is_space(text[pos])) {
            ++pos;
        }
    }

    if (is_newline(text[pos])) {
        while (is_newline(text[pos])) {
            ++pos;
        }
    }

    if (is_equal(text[pos])) {
        Token token = { EQUAL, NULL };
        ++pos;

        return token;
    }
    else if (is_alpha(text[pos])) {
        Token token = { DEF, malloc(0) };

        while (is_alpha(text[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text[pos];
            ++i;
            ++pos;
        }

        return token;
    }
    else if (is_quote(text[pos])) {
        Token token = { STRING, malloc(0) };
        ++pos;

        while (!is_quote(text[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text[pos];
            ++i;
            ++pos;
        }

        return token;
    }
    else if (is_singlequote(text[pos])) {
        Token token = { STRING, malloc(0) };
        ++pos;

        while (!is_singlequote(text[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text[pos];
            ++i;
            ++pos;
        }

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
