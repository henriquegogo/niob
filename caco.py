STRING, TEXT, EOL = 'STRING', 'TEXT', 'EOL'
text: str = ''

class Token():
    def __init__(self, type: str, pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length

# PARSER
def parser(tokens):
    for token in tokens:
        value = text[token.pos : token.pos + token.length]
        print("> ", token.type, value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_char(ch: str) -> bool:
    return ch != '\t' and ch != ' ' and not is_eol(ch)

def is_quote(ch: str) -> bool:
    return ch == '"' or ch == '\''

def lexer() -> list[Token]:
    tokens: list[Token] = []
    text_pos: int = 0

    while text_pos < len(text):
        while not is_char(text[text_pos]): text_pos += 1
        initial_pos: int = text_pos

        if is_quote(text[text_pos]):
            quote_char: str = text[text_pos]
            text_pos += 1
            while text[text_pos] != quote_char: text_pos += 1
            tokens.append(Token(TEXT, initial_pos, text_pos - initial_pos + 1))
        else:
            while is_char(text[text_pos]): text_pos += 1
            tokens.append(Token(STRING, initial_pos, text_pos - initial_pos))

        if is_eol(text[text_pos]) or is_eol(text[text_pos + 1]):
            tokens.append(Token(EOL))

        text_pos += 1

    return tokens

def read_file(filename: str):
    global text
    file = open(filename, 'r')
    lines: list = file.readlines()
    text = "".join(lines)

def main():
    read_file("script.caco");
    tokens = lexer();
    parser(tokens);

if __name__ == '__main__':
    main()
