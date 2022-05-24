IDENTIFIER, STRING, EOL = 'IDENTIFIER', 'STRING', 'EOL'

def _set(name, value):
    env['vars'][name] = value;

def _print(arg):
    value = arg if is_quote(arg[0]) else env['vars'][arg]
    print(value)

env = {
    'vars': {},
    'cmds': {
        'set': _set,
        'print': _print
    }
}

class Token():
    def __init__(self, type: str, pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length

# PARSER
def parser(tokens: list[Token], text: str):
    cmd, args = '', []

    for token in tokens:
        value = text[token.pos : token.pos + token.length]
        print("> ", token.type, value)

        if token.type == EOL:
            env['cmds'][cmd](*args)
            cmd, args = '', []
        elif token.type == IDENTIFIER and not cmd:
            cmd = value
        else:
            args.append(value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_char(ch: str) -> bool:
    return ch != '\t' and ch != ' ' and not is_eol(ch)

def is_quote(ch: str) -> bool:
    return ch == '"' or ch == '\''

def lexer(text: str) -> list[Token]:
    tokens: list[Token] = []
    pos: int = 0

    while pos + 1 < len(text):
        while not is_char(text[pos]): pos += 1
        init_pos: int = pos

        if is_quote(text[pos]):
            quote_char: str = text[pos]
            pos += 1
            while text[pos] != quote_char: pos += 1
            tokens.append(Token(STRING, init_pos, pos - init_pos + 1))
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
