IDENTIFIER, STRING, VARIABLE, EOL = 'IDENTIFIER', 'STRING', 'VARIABLE', 'EOL'

class Environment():
    class Variables(): pass

    def __init__(self):
        self.vars = self.Variables()

    def set(self, key, value):
        setattr(self.vars, key, value)

    def func(self, key, cmd):
        setattr(self, key, cmd)

    def sum(self, a, b):
        return float(a) + float(b)

class Token():
    def __init__(self, type: str, pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length

# PARSER
def parser(tokens: list, text: str):
    env = Environment()
    env.func('puts', print)

    cmds, cmds_args, args = [], [], []

    for token in tokens:
        value = text[token.pos : token.pos + token.length]
        print("> ", token.type, value)

        if token.type == EOL:
            cmds_args.insert(0, args.copy())
            cmd_return = None

            for i, cmd in enumerate(cmds):
                if cmd_return:
                    cmds_args[i].append(cmd_return)
                    cmd_return = None
                cmd_return = getattr(env, cmd)(*cmds_args[i])

            cmds, cmds_args, args = [], [], []
        elif token.type == IDENTIFIER and hasattr(env, value):
            if len(cmds) > 0:
                cmds_args.insert(0, args.copy())
                args = []
            cmds.insert(0, value)
        elif token.type == VARIABLE:
            args.append(getattr(env.vars, value))
        else:
            args.append(value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_char(ch: str) -> bool:
    return ch != '\t' and ch != ' ' and not is_eol(ch)

def lexer(text: str) -> list:
    tokens: list = []
    pos: int = 0

    while pos + 1 < len(text):
        while not is_char(text[pos]): pos += 1
        init_pos: int = pos

        if text[pos] == '"' or text[pos] == '\'':
            quote_char: str = text[pos]
            pos += 1
            while text[pos] != quote_char: pos += 1
            tokens.append(Token(STRING, init_pos + 1, pos - init_pos - 1))
        elif text[pos] == '$':
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
