#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char types[8][6] = {"IDF", "STR", "VAR", "CMT", "EOL", "BLCK", "EXPR"};
typedef enum {
    IDF, STR, VAR, CMT, EOL,
    BLCK, EXPR
} Type;

struct Token {
    Type type;
    char *value;
    struct Token *next;
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
char *eval(struct Token *token) {
    while (token->next) {
        token = token->next;

        printf("(%s) %s\n", types[token->type], token->value);

        if (token->type == CMT) {
        } else if (token->type == VAR) {
        //} else if (token.type == IDF && get_cmd(env, token.value)) {
        } else if (token->type == IDF || token->type == STR) {
        } else if (token->type == EXPR) {
        } else if (token->type == BLCK) {
        } else if (token->type == EOL) {
        }
    }

    return "";
}

int is_char(char ch) {
    return ch != ' ' && ch != '\t' && \
           ch != ';' && ch != '\n' && \
           ch != ')' && ch != '}';
}

char *slice(char *text, long start, long end) {
    char *value = malloc(end - start);
    strncpy(value, text + start, end - start);

    return value;
}

struct Token *lexer(char *text) {
    int text_length = strlen(text);
    long pos = 0;
    struct Token *tokens = malloc(sizeof(struct Token));
    tokens->next = NULL;

    while (pos + 1 < text_length) {
        while (text[pos] == ' ' || text[pos] == '\t'){
            if (pos + 1 == text_length) break;
            pos += 1;
        }

        if (text[pos] == '\n' || text[pos] == ';') {
            pos += 1;
            add_token(tokens, EOL, NULL);
        } else if (text[pos] == '$') {
            pos += 1;
            int init_pos = pos;
            while (pos < text_length && is_char(text[pos])) pos += 1;
            add_token(tokens, VAR, slice(text, init_pos, pos));
        } else if (text[pos] == '#') {
            int init_pos = pos;
            while (pos < text_length && text[pos] != '\n') pos += 1;
            add_token(tokens, CMT, slice(text, init_pos, pos));
        } else if (text[pos] == '(' || text[pos] == '{') {
            char open_char = text[pos];
            char close_char = open_char == '(' ? ')' : '}';
            Type token_type = open_char == '(' ? EXPR : BLCK;
            pos += 1;
            int init_pos = pos;
            int depth = 1;
            while (depth > 0) {
                if (text[pos] == open_char) depth += 1;
                else if (text[pos] == close_char) depth -= 1;
                pos += 1;
            }
            add_token(tokens, token_type, slice(text, init_pos, pos - 1));
        } else if (text[pos] == '"' || text[pos] == '\'') {
            char quote_char = text[pos];
            pos += 1;
            int init_pos = pos;
            while (text[pos] != quote_char) pos += 1;
            pos += 1;
            add_token(tokens, STR, slice(text, init_pos, pos - 1));
        } else if (is_char(text[pos])) {
            int init_pos = pos;
            while (pos < text_length && is_char(text[pos])) pos += 1;
            add_token(tokens, IDF, slice(text, init_pos, pos));
        }
    }

    add_token(tokens, EOL, NULL);

    return tokens;
}

char *interpret(char *text) {
    struct Token *tokens = lexer(text);
    return eval(tokens);
}

int main() {
    char *text = "                                              \n\
        # Niob is a language for scripting based on TCL and Ruby\n\
        set ten 10                                              \n\
        puts ((12 + $ten) + 56 )                                \n\
        message = 'Hello, world!'                               \n\
        puts $message                                           \n\
        if false { puts 'Should not print' }                    \n\
        if true {                                               \n\
            puts 'Should print'                                 \n\
            if true { puts 'Nested printed' }                   \n\
            if false { puts 'Nested not printed' }              \n\
        }                                                       \n\
        def the_end {                                           \n\
            puts 'Global var:' $message                         \n\
            puts 'END'                                          \n\
        }                                                       \n\
        the_end                                                 \n\
    ";
    interpret(text);

    return 0;
}
