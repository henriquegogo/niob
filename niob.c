#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char types[8][6] = {"IDF", "STR", "VAR", "CMT", "EOL", "EXPR", "BLCK_O", "BLCK_C"};
typedef enum {
    IDENTIFIER, STRING, VARIABLE, COMMENT, EOL,
    EXPRESSION, BLOCK_OPEN, BLOCK_CLOSE
} Type;

struct Node {
    char *key;
    char *value;
    struct Node *next;
};

struct Token {
    Type type;
    char *value;
    struct Token *next;
};

struct Result {
    char *content;
    struct Node *current;
};

void add_token(struct Token *token, Type type, char *value) {
    while (token->next) {
        token = token->next;
    }
    token->next = malloc(sizeof(struct Token));
    token->next->type = type;
    token->next->value = value;
    token->next->next = NULL;
}

// EVAL
void eval(struct Token *token) {

    while (token->next) {
        token = token->next;

        printf("(%s) %s\n", types[token->type], token->value);

        if (token->type == EOL) {
        } else if (token->type == EXPRESSION) {
        } else if (token->type == BLOCK_OPEN) {
            printf("open\n");
        } else if (token->type == BLOCK_CLOSE) {
            printf("close\n");
        } else if (token->type == IDENTIFIER) {
        } else if (token->type == VARIABLE) {
        } else if (token->type == COMMENT) {
        } else {
        }
    }
}

// LEXER
int is_eol(char ch) {
    return ch == '\n' || ch == '\r';
}

int is_closed(char ch) {
    return ch == ';' || ch == ')' || ch == '}';
}

int is_space(char ch) {
    return ch == ' ' || ch == '\t';
}

int is_char(char ch) {
    return !is_space(ch) && !is_eol(ch) && !is_closed(ch);
}

char *slice(char *text, long start, long end) {
    char *value = &text[start];
    value[end - start] = '\0';

    return value;
}

struct Token *lexer(char *text) {
    int text_length = strlen(text);
    long pos = 0;
    struct Token *tokens = malloc(sizeof(struct Token));
    tokens->next = NULL;

    while (pos < text_length) {
        while (is_space(text[pos])) pos += 1;
        long init_pos = pos;
        
        if (text[pos] == '"' || text[pos] == '\'') {
            init_pos = pos + 1;
            char quote_char = text[pos];
            pos += 1;
            while (text[pos] != quote_char) pos += 1;
            add_token(tokens, STRING, slice(text, init_pos, pos));
        } else if (text[pos] == '(') {
            add_token(tokens, EXPRESSION, NULL);
        } else if (text[pos] == ')') {
            add_token(tokens, EOL, NULL);
            pos += 1;
        } else if (text[pos] == '{') {
            add_token(tokens, BLOCK_OPEN, NULL);
        } else if (text[pos] == '}') {
            add_token(tokens, EOL, NULL);
            add_token(tokens, BLOCK_CLOSE, NULL);
        } else if (text[pos] == '#') {
            add_token(tokens, COMMENT, NULL);
            while (!is_eol(text[pos])) pos += 1;
        } else if (text[pos] == '$') {
            init_pos = pos + 1;
            while (is_char(text[pos])) pos += 1;
            add_token(tokens, VARIABLE, slice(text, init_pos, pos));
        } else if (is_char(text[pos])) {
            while (is_char(text[pos])) pos += 1;
            add_token(tokens, IDENTIFIER, slice(text, init_pos, pos));
        } else {
            add_token(tokens, EOL, NULL);
        }

        if (is_eol(text[pos]) || is_closed(text[pos])) {
            add_token(tokens, EOL, NULL);
        }

        pos += 1;
    }

    return tokens;
}

char *read_file(char *filename) {
    FILE *file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    long text_length = ftell(file);
    rewind(file);

    char *text = malloc(text_length * sizeof(char));
    fread(text, text_length, 1, file);
    fclose(file);
    
    return text;
}

int main() {
    char *text = read_file("script.nio");
    struct Token *tokens = lexer(text);
    eval(tokens);

    return 0;
}
