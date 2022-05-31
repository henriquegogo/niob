#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long text_length = 0;
long pos = 0;
long tokens_length = 0;

typedef enum {
    IDENTIFIER, STRING, NUMBER, EOL
} Type;

typedef struct {
    Type type;
    long pos;
    long length;
} Token;

// PARSER
void parser(Token *tokens, char *text) {
    char *cmd = "";

    for (int i = 0; i < tokens_length; i++) {
        char *value = &text[tokens[i].pos];
        value[tokens[i].length] = '\0';

        char types[4][16] = {"IDENTIFIER", "STRING", "NUMBER", "EOL"};
        printf("> %s %s\n", types[tokens[i].type], value);

        if (tokens[i].type == EOL) {
        }
        else if (tokens[i].type == IDENTIFIER && !cmd) {
        }
        else if (strcmp(cmd, "set")) {
        }
        else if (tokens[i].type == IDENTIFIER) {
        }
        else {
        }
    }
}

// LEXER
int is_eol(char ch) {
    return ch == '\n' || ch == '\r';
}

int is_number(char ch) {
    return ch >= '0' && ch <= '9';
}

int is_char(char ch) {
    return ch != '\t' && ch != ' ' && !is_eol(ch);
}

int is_quote(char ch) {
    return ch == '"' || ch == '\'';
}

Token *lexer(char *text) {
    Token *tokens = malloc(0);
    long token_index = 0;
    
    while (pos + 1 < text_length) {
        tokens = realloc(tokens, (token_index + 1) * sizeof(Token));
        while (!is_char(text[pos])) pos += 1;
        long init_pos = pos;

        if (is_quote(text[pos])) {
            char quote_char = text[pos];
            pos += 1;
            while (text[pos] != quote_char) pos += 1;
            tokens[token_index++] = (Token){ STRING, init_pos + 1, pos - init_pos - 1 };
        }
        else if (is_number(text[pos])) {
            while (is_number(text[pos])) pos += 1;
            tokens[token_index++] = (Token){ NUMBER, init_pos, pos - init_pos };
        }
        else {
            while (is_char(text[pos])) pos += 1;
            tokens[token_index++] = (Token){ IDENTIFIER, init_pos, pos - init_pos };
        }

        if (is_eol(text[pos]) || is_eol(text[pos + 1])) {
            tokens = realloc(tokens, (token_index + 1) * sizeof(Token));
            tokens[token_index++] = (Token){ EOL };
        }

        pos += 1;
    }

    tokens_length = token_index;

    return tokens;
}

char *read_file(char *filename) {
    FILE *file = fopen(filename, "r");

    fseek(file, 0, SEEK_END);
    text_length = ftell(file);
    rewind(file);

    char *text = malloc(text_length * sizeof(char));
    fread(text, text_length, 1, file);
    fclose(file);
    
    return text;
}

int main() {
    char *text = read_file("script.caco");
    Token *tokens = lexer(text);
    parser(tokens, text);

    free(tokens);
    free(text);

    return 0;
}
