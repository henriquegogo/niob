#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "niob.h"

typedef enum { IDF, STR, CMT, EOL, BLCK, EXPR } Type;

struct Token {
    Type type;
    int start;
    int end;
    struct Token *next;
};

struct TokenCache {
    char *key;
    struct Token *token;
    struct TokenCache *next;
};

struct Command {
    char *key;
    char *(*cmd)();
    char *body;
    struct Command *next;
};

struct Variable {
    char *key;
    char *value;
    struct Variable *next;
};

struct TokenCache *token_cache;

struct Command *commands;

struct Variable *variables;

// Struct functions (some of them are public and listed in the end of file)
void add_token(struct Token *token, Type type, int start, int end) {
    while (token->next) {
        token = token->next;
    }
    token->next = malloc(sizeof(struct Token));
    token->next->type = type;
    token->next->start = start;
    token->next->end = end;
}

struct Command *get_cmd(char *key) {
    struct Command *command = commands;
    while (command->next) {
        command = command->next;
        if (strcmp(command->key, key) == 0) return command;
    }
    return NULL;
}

// Helpers
char *slice(char *text, long start, long end) {
    char *value = malloc(end - start);
    strncpy(value, text + start, end - start);
    return value;
}

char *join(int argc, char **argv, char *separator) {
    int charc = 0;
    for (int i = 0; i < argc; i++) {
        if (i > 0 && separator[0]) charc += strlen(separator);
        if (argv[i]) charc += strlen(argv[i]) + 1;
    }
    char *output = malloc(charc);
    for (int i = 0; i < argc; i++) {
        if (i > 0 && separator[0]) strcat(output, separator);
        if (argv[i]) strcat(output, argv[i]);
    }
    return output;
}

char *itostr(int number) {
    char *output = malloc(sizeof(int));
    sprintf(output, "%d", number);
    return output;
}

int is_char(char ch) {
    return ch != ' ' && ch != '\t' && ch != '\n' && ch != ';' && \
           ch != '(' && ch != ')' && ch != '{' && ch != '}';
}

int is_alnum(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '_';
}

// Tokenizer and Parser
char *interpret(struct Token *token, char *text) {
    char *cmd = malloc(1);
    char **argv = malloc(1024);
    int argc = 0;

    while (token->next) {
        token = token->next;
        char *token_value = malloc(token->end - token->start);
        strncpy(token_value, text + token->start, token->end - token->start);

        if (token->type == CMT) {
        } else if (token->type == IDF) {
            if (!is_alnum(token_value[0]) && is_alnum(token_value[1])) {
                char *eval_expr = malloc(token->end - token->start + 1);
                sprintf(eval_expr, "%c %s", token_value[0], token_value + 1);
                argv[argc++] = niob_eval(eval_expr);
            } else if (!cmd[0] && get_cmd(token_value)) cmd = token_value;
            else argv[argc++] = token_value;
        } else if (token->type == STR || token->type == BLCK) {
            argv[argc++] = token_value;
        } else if (token->type == EXPR) {
            argv[argc++] = niob_eval(token_value);
        } else if (token->type == EOL) {
            char *output = malloc(1);
            struct Command *command = get_cmd(cmd);
            if (command) {
                if (command->body) argv[argc++] = command->body;
                output = strdup(command->cmd(cmd, argc, argv));
            } else if (argv[0]) output = join(argc, argv, " ");
            cmd = "";
            while (argc > 0) argv[--argc] = "";
            if (strlen(output) > 0) return output;
        }
    }

    return "";
}

struct Token *lexer(char *text) {
    int length = strlen(text);
    long pos = 0;
    struct Token *tokens = malloc(sizeof(struct Token));

    while (pos + 1 < length) {
        while (text[pos] == ' ' || text[pos] == '\t') {
            if (pos + 1 == length) break;
            pos += 1;
        }

        if (text[pos] == '\n' || text[pos] == ';') {
            pos += 1;
            add_token(tokens, EOL, 0, 0);
        } else if (text[pos] == '#') {
            int start = pos;
            while (pos < length && text[pos] != '\n') pos += 1;
            add_token(tokens, CMT, start, pos);
        } else if (text[pos] == '(' || text[pos] == '{') {
            char open_char = text[pos];
            char close_char = open_char == '(' ? ')' : '}';
            Type token_type = open_char == '(' ? EXPR : BLCK;
            pos += 1;
            int start = pos;
            int depth = 1;
            while (depth > 0) {
                if (text[pos] == open_char) depth += 1;
                else if (text[pos] == close_char) depth -= 1;
                pos += 1;
            }
            add_token(tokens, token_type, start, pos - 1);
        } else if (text[pos] == '"' || text[pos] == '\'') {
            char quote_char = text[pos];
            pos += 1;
            int start = pos;
            while (text[pos] != quote_char) pos += 1;
            pos += 1;
            add_token(tokens, STR, start, pos - 1);
        } else if (is_char(text[pos])) {
            int start = pos;
            while (pos < length && is_char(text[pos])) pos += 1;
            add_token(tokens, IDF, start, pos);
        }
    }

    add_token(tokens, EOL, 0, 0);

    return tokens;
}

// Built-in functions / commands
char *builtin_return(char *cmd, int argc, char **argv) {
    char *input = join(argc, argv, " ");
    return niob_eval(input);
}

char *builtin_if(char *cmd, int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "elseif") == 0) i++;
        if (strlen(argv[i]) > 0 && strcmp(argv[i], "false") != 0) {
            return niob_eval(argv[++i]);
        } else i++;
    }
    return "";
}

char *builtin_while(char *cmd, int argc, char **argv) {
    while (strcmp(niob_eval(argv[0]), "false") != 0) {
        if (strlen(niob_eval(argv[1])) > 0) break;
    }
    return "";
}

char *builtin_def(char *cmd, int argc, char **argv) {
    niob_def(argv[0], builtin_return, argv[argc - 1]);
    return "";
}

char *builtin_set(char *cmd, int argc, char **argv) {
    char *value = argv[2] ? join(argc, argv, " ") + strlen(argv[0]) + 1 :
        argv[1];
    niob_set(argv[0], value);
    return "";
}

char *builtin_get(char *cmd, int argc, char **argv) {
    return niob_get(argv[0]);
}

char *builtin_del(char *cmd, int argc, char **argv) {
    niob_del(argv[0]);
    return "";
}

char *builtin_math(char *cmd, int argc, char **argv) {
    long a = strtol(argv[0], NULL, 10);
    long b = strtol(argv[1], NULL, 10);
    char *output = malloc(64);
    sprintf(output, "%ld",
            strcmp(cmd, "+") == 0 ? a + b :
            strcmp(cmd, "-") == 0 ? a - b :
            strcmp(cmd, "*") == 0 ? a * b :
            strcmp(cmd, "/") == 0 ? a / b : 0
           );
    return output;
}

char *builtin_operators(char *cmd, int argc, char **argv) {
    int is_equal = strcmp(argv[0], argv[1]) == 0;
    int a_is_true = strcmp(argv[0], "false") != 0;
    int b_is_true = strcmp(argv[1], "false") != 0;
    long a = strtol(argv[0], NULL, 10);
    long b = strtol(argv[1], NULL, 10);
    return (
            strcmp(cmd, "&&") == 0 && a_is_true && b_is_true ||
            strcmp(cmd, "||") == 0 && (a_is_true || b_is_true) ||
            strcmp(cmd, "==") == 0 && is_equal ||
            strcmp(cmd, "!=") == 0 && !is_equal ||
            strcmp(cmd, ">=") == 0 && a >= b ||
            strcmp(cmd, "<=") == 0 && a <= b ||
            strcmp(cmd, ">") == 0 && a > b ||
            strcmp(cmd, "<") == 0 && a < b ?
            "true" : "false"
           );
}

char *builtin_puts(char *cmd, int argc, char **argv) {
    char *output = join(argc, argv, " ");
    printf("%s\n", output);
    return "";
}

char *builtin_concat(char *cmd, int argc, char **argv) {
    return join(argc, argv, "");
}

char *builtin_len(char *cmd, int argc, char **argv) {
    int length = strlen(join(argc, argv, " "));
    return itostr(length);
}

char *builtin_item(char *cmd, int argc, char **argv) {
    int current_i = 1;
    int desired_i = atoi(argv[1]);
    char *separator = argv[2] ? argv[2] : " ";
    char *item = strtok(argv[0], separator);
    char *output = item;
    while (item != NULL) {
        output = item;
        if (current_i++ == desired_i) break;
        item = strtok(NULL, separator);
    }
    return output;
}

char *builtin_size(char *cmd, int argc, char **argv) {
    char *separator = argv[2] ? argv[2] : " ";
    char *item = strtok(argv[0], separator);
    int count = 0;
    while (item != NULL) {
        count++;
        item = strtok(NULL, separator);
    }
    return itostr(count);
}

// API
void niob_init() {
    token_cache = malloc(sizeof(struct TokenCache));
    commands = malloc(sizeof(struct Command));
    variables = malloc(sizeof(struct Variable));

    niob_def("return", builtin_return, NULL);
    niob_def("if", builtin_if, NULL);
    niob_def("?", builtin_if, NULL);
    niob_def("while", builtin_while, NULL);
    niob_def("def", builtin_def, NULL);
    niob_def("set", builtin_set, NULL);
    niob_def("get", builtin_get, NULL);
    niob_def("=", builtin_set, NULL);
    niob_def("$", builtin_get, NULL);
    niob_def("del", builtin_del, NULL);
    niob_def("+", builtin_math, NULL);
    niob_def("-", builtin_math, NULL);
    niob_def("*", builtin_math, NULL);
    niob_def("/", builtin_math, NULL);
    niob_def("&&", builtin_operators, NULL);
    niob_def("||", builtin_operators, NULL);
    niob_def("==", builtin_operators, NULL);
    niob_def("!=", builtin_operators, NULL);
    niob_def(">=", builtin_operators, NULL);
    niob_def("<=", builtin_operators, NULL);
    niob_def(">", builtin_operators, NULL);
    niob_def("<", builtin_operators, NULL);
    niob_def("puts", builtin_puts, NULL);
    niob_def("concat", builtin_concat, NULL);
    niob_def("len", builtin_len, NULL);
    niob_def("item", builtin_item, NULL);
    niob_def("size", builtin_size, NULL);
}

char *niob_eval(char *text) {
    struct Token *tokens = NULL;
    struct TokenCache *cache = token_cache;
    while (cache->next) {
        cache = cache->next;
        if (strcmp(cache->key, text) == 0) tokens = cache->token;
    }
    if (!tokens) {
        struct TokenCache *cache = token_cache;
        while (cache->next) cache = cache->next;
        cache->next = malloc(sizeof(struct TokenCache));
        cache->next->key = text;
        cache->next->token = tokens = lexer(text);
    }
    return interpret(tokens, text);
}

void niob_def(char *key, char *(*cmd)(), char *body) {
    struct Command *command = commands;
    while (command->next) {
        command = command->next;
        if (strcmp(command->key, key) == 0) {
            command->cmd = cmd;
            command->body = body;
            return;
        }
    }
    command->next = malloc(sizeof(struct Command));
    command->next->key = key;
    command->next->cmd = cmd;
    command->next->body = body;
}

void niob_set(char *key, char *value) {
    struct Variable *variable = variables;
    while (variable->next) {
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) {
            variable->value = value;
            return;
        }
    }
    variable->next = malloc(sizeof(struct Variable));
    variable->next->key = key;
    variable->next->value = value;
}

char *niob_get(char *key) {
    struct Variable *variable = variables;
    while (variable->next) {
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) return variable->value;
    }
    return NULL;
}

void niob_del(char *key) {
    struct Variable *variable = variables;
    while (variable->next) {
        struct Variable *old_variable = variable;
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) {
            free(variable->key);
            free(variable->value);
            old_variable->next = variable->next;
            variable = old_variable;
        }
    }
}
