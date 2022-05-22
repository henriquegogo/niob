#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long text_length = 0;
long text_pos = 0;
long tokens_length = 0;
char types[3][16] = {"STRING", "TEXT", "EOL"};

typedef enum {
    STRING, TEXT, EOL
} Type;

typedef struct {
    Type type;
    char *value;
} Token;

typedef struct {
    Token token;
    struct AST *children;
} AST;

// PARSER
AST *parser(Token *tokens) {
    for (int i = 0; i < tokens_length; i++) {
        printf("> %s %s\n", types[tokens[i].type], tokens[i].value);

        switch (tokens[i].type) {
            case STRING:
                break;
            case TEXT:
                break;
            default:
                break;
        }
    }
    
    return &((AST) {});
}

// LEXER
int is_eol(char ch) {
    return ch == '\n' || ch == '\r';
}

int is_char(char ch) {
    return ch != '\t' && ch != ' ' && !is_eol(ch);
}

int is_quote(char ch) {
    return ch == '"' || ch == '\'';
}

Token new_token(int token_type, char *text, int initial_pos) {
    long chars_size = text_pos - initial_pos;

    Token token = { token_type, malloc(chars_size * sizeof(char)) };
    memcpy(token.value, &text[initial_pos], chars_size);
    token.value[chars_size] = '\0';

    return token;
}

Token *lexer(char *text) {
    Token *tokens = malloc(0);
    long token_index = 0;
    
    while (text_pos < text_length) {
        tokens = realloc(tokens, (token_index + 1) * sizeof(Token));
        while (!is_char(text[text_pos])) text_pos++;
        long initial_pos = text_pos;

        if (is_quote(text[text_pos])) {
            char quote_char = text[text_pos++];
            while (text[text_pos] != quote_char) text_pos++;
            tokens[token_index++] = new_token(TEXT, text, initial_pos + 1);
        }
        else {
            while (is_char(text[text_pos])) text_pos++;
            tokens[token_index++] = new_token(STRING, text, initial_pos);
        }

        if (is_eol(text[text_pos]) || is_eol(text[text_pos + 1])) {
            tokens = realloc(tokens, (token_index + 1) * sizeof(Token));
            tokens[token_index++] = (Token){ EOL };
        }

        text_pos++;
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

int main() {
    char *text = read_file("script.caco");
    Token *tokens = lexer(text);
    free(text);
    AST *ast = parser(tokens);
    free(tokens);

    return 0;
}
