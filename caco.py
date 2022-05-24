IDENTIFIER, STRING, NUMBER, EOL = 'IDENTIFIER', 'STRING', 'NUMBER', 'EOL'

def _set(key, value):
    env['vars'][key] = value

env = {
    'vars': {},
    'cmds': {
        'set': _set,
        'print': print
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
        elif cmd == 'set' and not len(args):
            args.append(value)
        elif token.type == IDENTIFIER:
            args.append(env['vars'][value])
        else:
            args.append(value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_number(ch: str) -> bool:
    return ch >= '0' and ch <= '9'

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
            tokens.append(Token(STRING, init_pos + 1, pos - init_pos - 1))
        elif is_number(text[pos]):
            while is_number(text[pos]): pos += 1
            tokens.append(Token(NUMBER, init_pos, pos - init_pos))
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
