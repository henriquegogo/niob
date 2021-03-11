#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long text_length = 0;
long text_pos = 0;
long tokens_length = 0;

typedef enum {
    NONE, STRING, VALUE, EQUAL, VAR, FUNC, ARG
} Type;

typedef struct {
    Type type;
    char *value;
} Token;

// PARSER
void parser(Token *tokens) {
    for (int i = 0; i <= tokens_length; i++) {
        Type token_type = tokens[i].type;
        char *token_value = tokens[i].value;

        if (token_type == VAR) {
        }
        else if (token_type == FUNC) {
            printf("printing ");
        }

        printf("> %s\n", token_value);
    }
}

// LEXER
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

int is_numeric(char ch) {
    return ch >= '0' && ch <= '9';
}

int is_quote(char ch) {
    return ch == '"' || ch == '\'';
}

int is_parentheses_open(char ch) {
    return ch == '(';
}

int is_parentheses_close(char ch) {
    return ch == ')';
}

Token next_token(char *text) {
    while (is_space(text[text_pos])) ++text_pos;
    while (is_newline(text[text_pos])) ++text_pos;

    if (is_equal(text[text_pos])) {
        ++text_pos;
        Token token = { EQUAL, NULL };

        return token;
    }
    else if (is_alpha(text[text_pos])) {
        long initial_text_pos = text_pos;
        while (is_alpha(text[text_pos]) || is_numeric(text[text_pos])) ++text_pos;
        long chars_size = text_pos - initial_text_pos;

        Type token_type;
        if (is_parentheses_open(text[text_pos])) token_type = FUNC;
        else if (is_parentheses_close(text[text_pos])) token_type = ARG;
        else token_type = VAR;

        Token token = { token_type, malloc(chars_size * sizeof(char)) };
        memcpy(token.value, &text[initial_text_pos], chars_size);
        token.value[chars_size] = '\0';

        return token;
    }
    else if (is_quote(text[text_pos])) {
        char quote_char = text[text_pos];
        long initial_text_pos = ++text_pos;
        while (text[text_pos] != quote_char) ++text_pos;
        long chars_size = text_pos - initial_text_pos;

        Token token = { STRING, malloc(chars_size * sizeof(char)) };
        memcpy(token.value, &text[initial_text_pos], chars_size);
        token.value[chars_size] = '\0';

        return token;
    }

    Token token = { NONE, NULL };

    return token;
}

Token *lexer(char *text) {
    Token *tokens;
    long token_index = 0;
    
    while (text_pos < text_length) {
        tokens = realloc(tokens, (token_index + 1) * sizeof(Token));
        Token token = next_token(text);
        
        if (token.type != NONE) {
            tokens[token_index] = token;
            ++token_index;
        }

        ++text_pos;
    }

    tokens_length = token_index;

    return tokens;
}

char *read_file(char *filename) {
    FILE *file = fopen(filename, "r");
    char *text;

    fseek(file, 0, SEEK_END);
    text_length = ftell(file);
    rewind(file);

    text = malloc(text_length * sizeof(char));
    fread(text, text_length, 1, file);
    fclose(file);

    return text;
}

Token *load(char *filename) {
    char *text = read_file(filename);
    Token *tokens = lexer(text);
    free(text);

    return tokens;
}

int main() {
    Token *tokens = load("script.caco");
    parser(tokens);
    free(tokens);

    return 0;
}
