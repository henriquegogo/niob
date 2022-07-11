#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char types[8][6] = {"IDF", "STR", "VAR", "CMT", "EOL", "BLCK", "EXPR"};
typedef enum { IDF, STR, VAR, CMT, EOL, BLCK, EXPR } Type;

struct Token {
    Type type;
    char *value;
    struct Token *next;
};

struct Command {
    char *key;
    char *(*cmd)();
    struct Command *next;
};

struct Variable {
    char *key;
    char *value;
    struct Variable *next;
};

struct Env {
    struct Command *commands;
    struct Variable *variables;
};

struct Env *env;

char *interpret(char *text);

void add_token(struct Token *token, Type type, char *value) {
    while (token->next) {
        token = token->next;
    }
    token->next = malloc(sizeof(struct Token));
    token->next->type = type;
    token->next->value = value;
    token->next->next = NULL;
}

void set_cmd(char *key, char *(*cmd)()) {
    struct Command *command = env->commands;
    while (command->next != NULL) {
        command = command->next;
        if (strcmp(command->key, key) == 0) {
            command->cmd = cmd;
            return;
        }
    }
    command->next = malloc(sizeof(struct Command));
    command->next->key = key;
    command->next->cmd = cmd;
    command->next->next = NULL;
}

void *get_cmd(char *key) {
    struct Command *command = env->commands;
    while (command->next != NULL) {
        command = command->next;
        if (strcmp(command->key, key) == 0) return command->cmd;
    }
    return NULL;
}

void set_var(char *key, char *value) {
    struct Variable *variable = env->variables;
    while (variable->next != NULL) {
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) {
            variable->value = value;
            return;
        }
    }
    variable->next = malloc(sizeof(struct Variable));
    variable->next->key = key;
    variable->next->value = value;
    variable->next->next = NULL;
}

char *get_var(char *key) {
    struct Variable *variable = env->variables;
    while (variable->next != NULL) {
        variable = variable->next;
        if (strcmp(variable->key, key) == 0) return variable->value;
    }
    return "";
}

char *eval(struct Token *token) {
    char *cmd_key = malloc(1);
    char **cmd_args = malloc(1);
    int cmd_args_count = 0;

    while (token->next) {
        token = token->next;

        if (token->type == CMT) {
        } else if (token->type == VAR) {
            char *value = get_var(token->value);
            cmd_args[cmd_args_count++] = strdup(value);
        } else if (token->type == IDF && get_cmd(token->value)) {
            cmd_key = strdup(token->value);
        } else if (token->type == IDF || token->type == STR) {
            cmd_args[cmd_args_count++] = strdup(token->value);
        } else if (token->type == EXPR) {
            cmd_args[cmd_args_count++] = strdup(interpret(token->value));
        } else if (token->type == BLCK) {
            cmd_args[cmd_args_count++] = strdup(token->value);
        } else if (token->type == EOL) {
            char *cmd_return = malloc(1);
            char *(*cmd)() = get_cmd(cmd_key);
            if (cmd) cmd_return = strdup(cmd(cmd_key, cmd_args));
            else if (cmd_args[0]) cmd_return = strdup(cmd_args[0]);
            cmd_key = strdup("");
            while (cmd_args_count > 0) cmd_args[--cmd_args_count] = strdup("");
            if (strlen(cmd_return) > 0) return cmd_return;
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

char *builtin_if(char *cmd, char **argv) {
    return (
            strlen(argv[0]) > 0 &&
            strcmp(argv[0], "false") != 0 &&
            strcmp(argv[0], "0") != 0 ?
            interpret(argv[1]) : ""
           );
}

char *builtin_def(char *cmd, char **argv) {
    set_cmd(argv[0], interpret); // how to lambda interpret(argv[1])
    return "";
}

char *builtin_set(char *cmd, char **argv) {
    set_var(argv[0], argv[1]);
    return "";
}

char *builtin_math(char *cmd, char **argv) {
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

char *builtin_operators(char *cmd, char **argv) {
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

char *builtin_puts(char *cmd, char **argv) {
    while (*argv) {
        printf("%s", *argv++);
        printf(*argv ? " " : "\n");
    }
    return "";
}

int main() {
    env = malloc(sizeof(struct Env));
    env->commands = malloc(sizeof(struct Command));
    env->commands->key = "";
    env->commands->next = NULL;
    env->variables = malloc(sizeof(struct Variable));
    env->variables->key = "";
    env->variables->next = NULL;

    set_cmd("if", builtin_if);
    //set_cmd("def", builtin_def);
    set_cmd("set", builtin_set);
    set_cmd("puts", builtin_puts);
    set_cmd("+", builtin_math);
    set_cmd("-", builtin_math);
    set_cmd("*", builtin_math);
    set_cmd("/", builtin_math);
    set_cmd("&&", builtin_operators);
    set_cmd("||", builtin_operators);
    set_cmd("==", builtin_operators);
    set_cmd("!=", builtin_operators);
    set_cmd(">=", builtin_operators);
    set_cmd("<=", builtin_operators);
    set_cmd(">", builtin_operators);
    set_cmd("<", builtin_operators);
    set_cmd("=", get_cmd("set"));

    char *text = "                                               \n\
        # Niob is a language for scripting based on TCL and Ruby \n\
        set ten 10                                               \n\
        puts (2 * ((12 + $ten) + 56 ))                           \n\
        message = 'Hello, world!'                                \n\
        puts $message                                            \n\
        if false { puts 'Should not print' }                     \n\
        if (2 > 1) {                                             \n\
            puts 'Should print'                                  \n\
            if true { puts 'Nested printed' }                    \n\
            if false { puts 'Nested not printed' }               \n\
        }                                                        \n\
        def the_end {                                            \n\
            puts 'Global var:' $message                          \n\
            puts 'END'                                           \n\
        }                                                        \n\
        the_end                                                  \n\
    ";
    interpret(text);

    return 0;
}
