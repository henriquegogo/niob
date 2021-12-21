#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long text_length = 0;
long text_pos = 0;
long tokens_length = 0;

typedef struct {
    char *content;
    long length;
} Text;

typedef enum {
    NONE, NEWLINE, NUMBER, OPERATOR, STRING,
    VALUE, EQUAL, VAR, FUNCTION, ARGUMENT
} Type;

typedef struct {
    Type type;
    char *value;
} Token;

typedef struct {
    char *key;
    char *value;
} KeyValue;

// AUXILIAR
char* substr(char *text, long initial_text_pos, long chars_size) {
    char *value = malloc(chars_size * sizeof(char));
    memcpy(value, &text[initial_text_pos], chars_size);
    value[chars_size] = '\0';

    return value;
}

char* type_name(int i) {
    char *types[] = {
        "NONE", "NEWLINE", "NUMBER", "OPERATOR", "STRING",
        "VALUE", "EQUAL", "VAR", "FUNCTION", "ARGUMENT" 
    };
    return types[i];
}

// PARSER
void parser(Token *tokens) {
    long stack_length = 0;
    long a = 0, b = 0;

    for (int i = 0; i <= tokens_length; i++) {
        int type = tokens[i].type;
        char *value = tokens[i].value;

        printf("> %s %s\n", type_name(type), value);

        if (type == NUMBER) {
            b = a;
            a = atoi(value);
        }
        else if (type == OPERATOR) {
            printf("VAL: %c\n", value[0]);
            if (value[0] == '+') {
                a = b + a;
                b = 0;
            }
            else if (value[0] == '-') {
                a = b - a;
                b = 0;
            }
        }
        else if (type == NEWLINE) {
            printf("RESULT: %ld\n", a);
        }
    }
}

// LEXER
int is_space(char ch) {
    return ch == ' ' || ch == '\t';
}

int is_newline(char ch) {
    return ch == '\n' || ch == '\r';
}

int is_numeric(char ch) {
    return ch >= '0' && ch <= '9';
}

int is_operator(char ch) {
    return ch == '+' || ch == '-' || ch == '=';
}

Token *lexer(char *text) {
    Token *tokens = malloc(sizeof(Token));
    
    while (text_pos < text_length) {
        tokens = realloc(tokens, (tokens_length + 1) * sizeof(Token));
        Token token = {};

        while (is_space(text[text_pos])) ++text_pos;

        if (is_newline(text[text_pos])) {
            while (is_newline(text[text_pos])) ++text_pos;
            token.type = NEWLINE;
        }
        else if (is_numeric(text[text_pos])) {
            long init_pos = text_pos;
            while (is_numeric(text[text_pos])) ++text_pos;
            token.type = NUMBER;
            token.value = substr(text, init_pos, text_pos - init_pos);
        }
        else if (is_operator(text[text_pos])) {
            long init_pos = text_pos;
            while (is_operator(text[text_pos])) ++text_pos;
            token.type = OPERATOR;
            token.value = substr(text, init_pos, text_pos - init_pos);
        }

        tokens[tokens_length++] = token;
        ++text_pos;
    }

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

int main(int argc, char *argv[]) {
    Token *tokens = load(argv[1]);
    parser(tokens);
    free(tokens);

    return 0;
}
