#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { IDF, STR, VAR, CMT, EOL, BLCK, EXPR } Type;

struct Token {
    Type type;
    char *value;
    struct Token *next;
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

struct Command *commands;

struct Variable *variables;

char *eval(char *text);

void add_token(struct Token *token, Type type, char *value) {
    while (token->next) {
        token = token->next;
    }
    token->next = malloc(sizeof(struct Token));
    token->next->type = type;
    token->next->value = value;
}

void set_cmd(char *key, char *(*cmd)(), char *body) {
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

struct Command *get_cmd(char *key) {
    struct Command *command = commands;
    while (command->next) {
        command = command->next;
        if (strcmp(command->key, key) == 0) return command;
    }
    return NULL;
}

void set_var(char *key, char *value) {
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

char *get_var(char *key) {
    struct Variable *variable = variables;
    while (variable->next) {
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) return variable->value;
    }
    return NULL;
}

char *del_var(char *key) {
    struct Variable *variable = variables;
    while (variable->next) {
        struct Variable *old_variable = variable;
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) {
            free(variable->key);
            free(variable->value);
            old_variable->next = variable->next;
            free(variable);
            variable = old_variable;
        }
    }
    return "";
}

char *slice(char *text, long start, long end) {
    char *value = malloc(end - start);
    strncpy(value, text + start, end - start);
    return value;
}

char *join(int argc, char **argv) {
    int charc = 0;
    for (int i = 0; i < argc; i++) {
        if (argv[i]) charc += strlen(argv[i]) + 1;
    }
    char *output = malloc(charc);
    for (int i = 0; i < argc; i++) {
        if (i > 0) strcat(output, " ");
        if (argv[i]) strcat(output, argv[i]);
    }
    return output;
}

char *interpret(struct Token *token) {
    char *cmd_key = malloc(1);
    char **argv = malloc(1);
    int argc = 0;

    while (token->next) {
        token = token->next;

        if (token->type == CMT) {
        } else if (token->type == VAR) {
            char *value = get_var(token->value);
            argv[argc++] = value ? strdup(value) : value;
        } else if (token->type == IDF && get_cmd(token->value)) {
            cmd_key = strdup(token->value);
        } else if (token->type == IDF || token->type == STR ||
                token->type == BLCK) {
            argv[argc++] = strdup(token->value);
        } else if (token->type == EXPR) {
            argv[argc++] = strdup(eval(token->value));
        } else if (token->type == EOL) {
            char *output = malloc(1);
            struct Command *command = get_cmd(cmd_key);
            if (command) {
                if (command->body) argv[argc++] = command->body;
                output = strdup(command->cmd(cmd_key, argc, argv));
            } else if (argv[0]) output = join(argc, argv);
            cmd_key = "";
            while (argc > 0) argv[--argc] = "";
            if (strlen(output) > 0) return output;
        }
    }

    return "";
}

int is_char(char ch) {
    return ch != ' ' && ch != '\t' && \
           ch != ';' && ch != '\n' && \
           ch != ')' && ch != '}';
}

struct Token *lexer(char *text) {
    int text_length = strlen(text);
    long pos = 0;
    struct Token *tokens = malloc(sizeof(struct Token));

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

char *eval(char *text) {
    struct Token *tokens = lexer(text);
    return interpret(tokens);
}

char *builtin_eval(char *cmd, int argc, char **argv) {
    eval(argv[0]);
    return "";;
}

char *builtin_if(char *cmd, int argc, char **argv) {
    for (int i = 0; i < argc; i += 2) {
        if (strcmp(argv[i], "elseif") == 0) i++;
        if (strlen(argv[i]) > 0 &&
            strcmp(argv[i], "false") != 0 &&
            strcmp(argv[i], "0") != 0) {

            return eval(argv[i + 1]);
        }
    }
    return "";
}

char *builtin_def(char *cmd, int argc, char **argv) {
    set_cmd(argv[0], builtin_eval, argv[argc - 1]);
    return "";
}

char *builtin_set(char *cmd, int argc, char **argv) {
    set_var(argv[0], argv[1]);
    return "";
}

char *builtin_delete(char *cmd, int argc, char **argv) {
    del_var(argv[0]);
    return "";
}

char *builtin_math(char *cmd, int argc, char **argv) {
    float a = strtof(argv[0], NULL);
    float b = strtof(argv[1], NULL);
    char *output = malloc(64);
    sprintf(output, "%.2f",
            strcmp(cmd, "+") == 0 ? a + b :
            strcmp(cmd, "-") == 0 ? a - b :
            strcmp(cmd, "*") == 0 ? a * b :
            strcmp(cmd, "/") == 0 ? a / b : 0
           );
    return output;
}

char *builtin_operators(char *cmd, int argc, char **argv) {
    int is_equal = strcmp(argv[0], argv[1]) == 0;
    int a_is_true = strcmp(argv[0], "true") == 0;
    int b_is_true = strcmp(argv[1], "true") == 0;
    float a = strtof(argv[0], NULL);
    float b = strtof(argv[1], NULL);
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
    char *output = join(argc, argv);
    printf("%s\n", output);
    return "";
}

void init() {
    commands = malloc(sizeof(struct Command));
    variables = malloc(sizeof(struct Variable));

    set_cmd("eval", builtin_eval, NULL);
    set_cmd("if", builtin_if, NULL);
    set_cmd("def", builtin_def, NULL);
    set_cmd("set", builtin_set, NULL);
    set_cmd("delete", builtin_delete, NULL);
    set_cmd("puts", builtin_puts, NULL);
    set_cmd("+", builtin_math, NULL);
    set_cmd("-", builtin_math, NULL);
    set_cmd("*", builtin_math, NULL);
    set_cmd("/", builtin_math, NULL);
    set_cmd("&&", builtin_operators, NULL);
    set_cmd("||", builtin_operators, NULL);
    set_cmd("==", builtin_operators, NULL);
    set_cmd("!=", builtin_operators, NULL);
    set_cmd(">=", builtin_operators, NULL);
    set_cmd("<=", builtin_operators, NULL);
    set_cmd(">", builtin_operators, NULL);
    set_cmd("<", builtin_operators, NULL);
    set_cmd("=", get_cmd("set")->cmd, NULL);
    set_cmd("?", get_cmd("if")->cmd, NULL);
}

int main(int argc, char **argv) {
    init();

    if (argc == 1) {
        char input[1024];
        while (1) {
            printf("niob> ");
            fgets(input,1024,stdin);
            eval(input);
        }
    } else if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        rewind(file);
        char *text = malloc(size);
        fread(text, size, 1, file);
        eval(text);
        fclose(file);
    }

    return 0;
}
