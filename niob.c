#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long text_length = 0;

char types[5][16] = {"IDENTIFIER", "STRING", "VARIABLE", "COMMENT", "EOL"};
typedef enum {
    IDENTIFIER, STRING, VARIABLE, COMMENT, EOL
} Type;

// TOKEN METHODS
typedef struct Token {
    Type type;
    long pos;
    long length;
    struct Token *next;
} Token;

void add_token(struct Token token, struct Token last) {
    while (token.next) {
        token = *token.next;
    }
    token.next = &last;
}

// NODE METHODS
typedef struct Node {
    char *key;
    char *value;
    struct Node *next;
} Node;

char *set_node(struct Node node, char *key, char *value) {
    while (node.next) {
        node = *node.next;
        if (strcmp(node.key, key)) {
            return node.value = value;
        }
    }
    node.next = &((Node){ key, value });
    return node.next->value;
}

// PARSER
void parser(struct Token token, char *text) {
    while (token.next) {
        token = *token.next;
        char *value = &text[token.pos];
        value[token.length] = '\0';
        printf("> %s %s\n", types[token.type], value);
    }
}

// LEXER
int is_eol(char ch) {
    return ch == '\n' || ch == '\r' || ch == ';';
}

int is_char(char ch) {
    return ch != '\t' && ch != ' ' && !is_eol(ch);
}

Token lexer(char *text) {
    long pos = 0;
    Token tokens = ((Token){});
    
    while (pos + 1 < text_length) {
        while (!is_char(text[pos])) pos += 1;
        long init_pos = pos;

        if (text[pos] == '"' || text[pos] == '\'') {
            char quote_char = text[pos];
            pos += 1;
            while (text[pos] != quote_char) pos += 1;
            add_token(tokens, ((Token){ STRING, init_pos + 1, pos - init_pos - 1 }));
        } else if (text[pos] == '$') {
            while (is_char(text[pos])) pos += 1;
            add_token(tokens, ((Token){ VARIABLE, init_pos + 1, pos - init_pos - 1 }));
        } else if (text[pos] == '#') {
            while (!is_eol(text[pos])) pos += 1;
            add_token(tokens, ((Token){ COMMENT, init_pos + 1, pos - init_pos - 1 }));
        } else {
            while (is_char(text[pos])) pos += 1;
            add_token(tokens, ((Token){ IDENTIFIER, init_pos, pos - init_pos }));
        }

        if (is_eol(text[pos]) || is_eol(text[pos + 1])) {
            add_token(tokens, ((Token){ EOL }));
        }

        pos += 1;
    }

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
    Token tokens = lexer(text);
    parser(tokens, text);

    free(text);

    return 0;
}
