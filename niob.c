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

struct Result {
    char *content;
    struct Node *current;
};

void add_node(struct Node *node, char *key, char *value) {
    while (node->next) {
        node = node->next;
    }
    node->next = malloc(sizeof(struct Node));
    node->next->key = key;
    node->next->value = value;
    node->next->next = NULL;
}

// EVAL
void eval(struct Node *token) {

    while (token->next) {
        token = token->next;

        printf("(%s) %s\n", token->key, token->value);
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

struct Node *lexer(char *text) {
    int text_length = strlen(text);
    long pos = 0;
    struct Node *tokens = malloc(sizeof(struct Node));
    tokens->next = NULL;

    while (pos < text_length) {
        while (is_space(text[pos])) pos += 1;
        long init_pos = pos;
        
        if (text[pos] == '"' || text[pos] == '\'') {
            init_pos = pos + 1;
            char quote_char = text[pos];
            pos += 1;
            while (text[pos] != quote_char) pos += 1;
            add_node(tokens, types[STRING], slice(text, init_pos, pos));
        } else if (text[pos] == '(') {
            add_node(tokens, types[EXPRESSION], NULL);
        } else if (text[pos] == ')') {
            add_node(tokens, types[EOL], NULL);
            pos += 1;
        } else if (text[pos] == '{') {
            add_node(tokens, types[BLOCK_OPEN], NULL);
        } else if (text[pos] == '}') {
            add_node(tokens, types[EOL], NULL);
            add_node(tokens, types[BLOCK_CLOSE], NULL);
        } else if (text[pos] == '#') {
            add_node(tokens, types[COMMENT], NULL);
            while (!is_eol(text[pos])) pos += 1;
        } else if (text[pos] == '$') {
            init_pos = pos + 1;
            while (is_char(text[pos])) pos += 1;
            add_node(tokens, types[VARIABLE], slice(text, init_pos, pos));
        } else if (is_char(text[pos])) {
            while (is_char(text[pos])) pos += 1;
            add_node(tokens, types[IDENTIFIER], slice(text, init_pos, pos));
        } else {
            add_node(tokens, types[EOL], NULL);
        }

        if (is_eol(text[pos]) || is_closed(text[pos])) {
            add_node(tokens, types[EOL], NULL);
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
    struct Node *tokens = lexer(text);
    eval(tokens);

    return 0;
}
