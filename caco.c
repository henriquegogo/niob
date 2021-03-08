#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

long pos = 0;

typedef enum {
    NONE, VALUE, EQUAL, VAR, FUNC, ARG
} Types;

typedef struct Token {
    Types type;
    char *value;
} Token;

typedef struct {
    char *content;
    long length;
} String;

bool is_space(char ch) {
    return ch == ' ' || ch == '\t';
}

bool is_equal(char ch) {
    return ch == '=';
}

bool is_newline(char ch) {
    return ch == '\n' || ch == '\r';
}

bool is_variable(char ch) {
    return ch >= 'A' && ch <= 'Z' \
        || ch >= 'a' && ch <= 'z' \
        || ch == '_';
}

bool is_quotation(char ch) {
    return ch == '\'' || ch == '"';
}

bool is_value(char ch) {
    return ch >= '0' && ch <= '9' \
        || ch >= 'A' && ch <= 'Z' \
        || ch >= 'a' && ch <= 'z' \
        || ch == '.' \
        || ch == ',' \
        || ch == ';' \
        || ch == ':' \
        || ch == '\\' \
        || ch == '/' \
        || ch == '-' \
        || ch == '_' \
        || ch == '#' \
        || ch == '$' \
        || ch == ' ' \
        || ch == '"' \
        || ch == '\'';
}

Token next_token(String text) {
    Token token;

    if (is_space(text.content[pos])) {
        while (is_space(text.content[pos])) {
            ++pos;
        }
    }

    if (is_newline(text.content[pos])) {
        while (is_newline(text.content[pos])) {
            ++pos;
        }
    }

    if (is_equal(text.content[pos])) {
        token.type = EQUAL;
        token.value = "";
        ++pos;

        return token;
    }
    else if (is_variable(text.content[pos])) {
        token.type = VAR;
        token.value = malloc(0);
        long i = 0;

        while (is_variable(text.content[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text.content[pos];
            ++i;
            ++pos;
        }

        return token;
    }
    else if (is_quotation(text.content[pos])) {
        token.type = VALUE;
        token.value = malloc(0);
        long i = 0;

        while (is_value(text.content[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text.content[pos];
            ++i;
            ++pos;
        }

        return token;
    }

    token.type = NONE;
    token.value = NULL;

    return token;
}

Token* lexer(String text) {
    Token *tokens;
    long i = 0;

    while (pos < text.length) {
        tokens = realloc(tokens, (i + 1) * sizeof(Token));
        Token token = next_token(text);
        tokens[i] = token;
        ++i;
        ++pos;
    }

    return tokens;
}

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
    Token *tokens = lexer(text);
    return text;
}

int main() {
    String text = load("script.caco");
    printf("%s", text.content);
}
