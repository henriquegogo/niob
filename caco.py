IDENTIFIER, STRING, VARIABLE, EOL = 'IDENTIFIER', 'STRING', 'VARIABLE', 'EOL'

class Environment():
    class Variables(): pass

    def __init__(self):
        self.vars = self.Variables()

    def set(self, key, value):
        setattr(self.vars, key, value)

    def func(self, key, cmd):
        setattr(self, key, cmd)

class Token():
    def __init__(self, type: str, pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length

# PARSER
def parser(tokens: list, text: str):
    env = Environment()
    env.func('puts', print)

    cmd, args = '', []

    for token in tokens:
        value = text[token.pos : token.pos + token.length]
        print("> ", token.type, value)

        if token.type == EOL:
            getattr(env, cmd)(*args)
            cmd, args = '', []
        elif token.type == IDENTIFIER and hasattr(env, value) and not cmd:
            cmd = value
        elif token.type == VARIABLE:
            args.append(getattr(env.vars, value))
        else:
            args.append(value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_char(ch: str) -> bool:
    return ch != '\t' and ch != ' ' and not is_eol(ch)

def is_quote(ch: str) -> bool:
    return ch == '"' or ch == '\''

def is_var_prefix(ch: str) -> bool:
    return ch == '$'

def lexer(text: str) -> list:
    tokens: list = []
    pos: int = 0

    while pos + 1 < len(text):
        while not is_char(text[pos]): pos += 1
        init_pos: int = pos

        if is_quote(text[pos]):
            quote_char: str = text[pos]
            pos += 1
            while text[pos] != quote_char: pos += 1
            tokens.append(Token(STRING, init_pos + 1, pos - init_pos - 1))
        elif is_var_prefix(text[pos]):
            while is_char(text[pos]): pos += 1
            tokens.append(Token(VARIABLE, init_pos + 1, pos - init_pos - 1))
        else:
            while is_char(text[pos]): pos += 1
            tokens.append(Token(IDENTIFIER, init_pos, pos - init_pos))

        if is_eol(text[pos]) or is_eol(text[pos + 1]):
            tokens.append(Token(EOL))

        pos += 1

    return tokens

def read_file(filename: str):
    file = open(filename, 'r')
    lines: list = file.readlines()
    return "".join(lines)

def main():
    text = read_file("script.caco")
    tokens = lexer(text)
    parser(tokens, text)

if __name__ == '__main__':
    main()
