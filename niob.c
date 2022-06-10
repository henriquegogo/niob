#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char types[8][6] = {"IDF", "STR", "VAR", "CMT", "EOL", "EXPR", "BLCK_O", "BLCK_C"};
typedef enum {
    IDENTIFIER, STRING, VARIABLE, COMMENT, EOL,
    EXPRESSION, BLOCK_OPEN, BLOCK_CLOSE
} Type;

typedef struct Node Node;
struct Node {
    char *key;
    char *value;
    Node *next;
};

typedef struct Result Result;
struct Result {
    char *content;
    struct Node *current;
};

char *set_node(struct Node node, char *key, char *value) {
    while (node.next) {
        node = *node.next;
        if (strcmp(node.key, key)) {
            return node.value = value;
        }
    }
    node.next = &((Node){ key, value });
    return NULL;
}

char *get_node(struct Node node, char *key) {
    while (node.next) {
        node = *node.next;
        if (strcmp(node.key, key)) {
            return node.value;
        }
    }
    return NULL;
}

void del_node(struct Node node, char *key) {
    while (node.next) {
        if (strcmp(node.next->key, key)) {
            node.next = node.next->next;
        }
        node = *node.next;
    }
}

void add_node(struct Node node, struct Node last) {
    while (node.next) {
        node = *node.next;
    }
    node.next = &last;
}

// EVAL
Node *variables = &(Node){};
Node *functions = &(Node){};

Result *eval(Node *token) {
    return NULL;
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

Node lexer(char *text) {
    int text_length = strlen(text);
    long pos = 0;
    Node tokens = (Node){};

    while (pos < text_length) {
        while (is_space(text[pos])) pos += 1;
        long init_pos = pos;
        
        if (text[pos] == '"' || text[pos] == '\'') {
            init_pos = pos + 1;
            char quote_char = text[pos];
            pos += 1;
            while (text[pos] != quote_char) pos += 1;
            add_node(tokens, ((Node){ types[STRING], slice(text, init_pos, pos) }));
            printf(">> %s\n", slice(text, init_pos, pos));
        }

        pos += 1;
    }
    /*
    
    while (pos < text_length) {
        while (is_space(text[pos])) pos += 1;
        long init_pos = pos;

        if (text[pos] == '"' || text[pos] == '\'') {
            init_pos = pos + 1;
            char quote_char = text[pos];
            pos += 1;
            while (text[pos] != quote_char) pos += 1;
            add_node(tokens, ((Node){ types[STRING], strncpy(text, text + init_pos, pos - init_pos) }));
        } else if (text[pos] == '(') {
            add_node(tokens, ((Node){ types[EXPRESSION] }));
        } else if (text[pos] == ')') {
            add_node(tokens, ((Node){ types[EOL] }));
            pos += 1;
        } else if (text[pos] == '{') {
            add_node(tokens, ((Node){ types[BLOCK_OPEN] }));
        } else if (text[pos] == '}') {
            add_node(tokens, ((Node){ types[EOL] }));
            add_node(tokens, ((Node){ types[BLOCK_CLOSE] }));
        } else if (text[pos] == '#') {
            add_node(tokens, ((Node){ types[COMMENT] }));
            while (!is_eol(text[pos])) pos += 1;
        } else if (text[pos] == '$') {
            init_pos = pos + 1;
            while (is_char(text[pos])) pos += 1;
            add_node(tokens, ((Node){ types[VARIABLE], strncpy(text, text + init_pos, pos - init_pos) }));
        } else if (is_char(text[pos])) {
            while (is_char(text[pos])) pos += 1;
            add_node(tokens, ((Node){ types[IDENTIFIER], strncpy(text, text + init_pos, pos - init_pos) }));
        } else {
            add_node(tokens, ((Node){ types[EOL] }));
        }

        if (is_eol(text[pos]) || is_closed(text[pos])) {
            add_node(tokens, ((Node){ types[EOL] }));
        }


        pos += 1;
    }

    return tokens;
    */
    return (Node){};
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
    Node tokens = lexer(text);
    //eval(tokens);

    return 0;
}
