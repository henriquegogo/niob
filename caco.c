#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *text;
long text_length = 0;
long text_pos = 0;
long tokens_length = 0;

typedef enum {
    STRING, TEXT, EOL
} Type;

typedef struct {
    Type type;
    long pos;
    long length;
} Token;

typedef struct {
    Token token;
    struct AST *children;
} AST;

// PARSER
AST *parser(Token *tokens) {
    for (int i = 0; i < tokens_length; i++) {
        char *value = &text[tokens[i].pos];
        value[tokens[i].length] = '\0';

        char types[3][16] = {"STRING", "TEXT", "EOL"};
        printf("> %s %s\n", types[tokens[i].type], value);

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

Token *lexer() {
    Token *tokens = malloc(0);
    long token_index = 0;
    
    while (text_pos < text_length) {
        tokens = realloc(tokens, (token_index + 1) * sizeof(Token));
        while (!is_char(text[text_pos])) text_pos++;
        long initial_pos = text_pos;

        if (is_quote(text[text_pos])) {
            char quote_char = text[text_pos++];
            while (text[text_pos] != quote_char) text_pos++;
            tokens[token_index++] = (Token){ TEXT, initial_pos, text_pos - initial_pos + 1 };
        }
        else {
            while (is_char(text[text_pos])) text_pos++;
            tokens[token_index++] = (Token){ STRING, initial_pos, text_pos - initial_pos };
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

void read_file(char *filename) {
    FILE *file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    text_length = ftell(file);
    rewind(file);

    text = malloc(text_length * sizeof(char));
    fread(text, text_length, 1, file);
    fclose(file);
}

int main() {
    read_file("script.caco");
    Token *tokens = lexer();
    AST *ast = parser(tokens);
    free(tokens);
    free(text);

    return 0;
}
