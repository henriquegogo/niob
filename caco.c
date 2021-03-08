#include <stdio.h>
#include <stdlib.h>

long pos = 0;

typedef enum {
    NONE, STRING, VALUE, EQUAL, DEF, FUNC, ARG
} Types;

typedef struct Token {
    Types type;
    char *value;
} Token;

typedef struct {
    char *content;
    long length;
} String;

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

Token next_token(String text) {
    long i = 0;

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
        Token token = { EQUAL, NULL };
        ++pos;

        return token;
    }
    else if (is_alpha(text.content[pos])) {
        Token token = { DEF, malloc(0) };

        while (is_alpha(text.content[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text.content[pos];
            ++i;
            ++pos;
        }

        return token;
    }
    else if (is_quote(text.content[pos])) {
        Token token = { STRING, malloc(0) };
        ++pos;

        while (!is_quote(text.content[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text.content[pos];
            ++i;
            ++pos;
        }

        return token;
    }
    else if (is_singlequote(text.content[pos])) {
        Token token = { STRING, malloc(0) };
        ++pos;

        while (!is_singlequote(text.content[pos])) {
            token.value = realloc(token.value, (i + 1) * sizeof(char));
            token.value[i] = text.content[pos];
            ++i;
            ++pos;
        }

        return token;
    }

    Token token = { NONE, NULL };

    return token;
}

Token* lexer(String text) {
    Token *tokens;
    long i = 0;

    while (pos < text.length) {
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
