IDENTIFIER, STRING, VARIABLE, COMMENT, BLOCK, EOL = \
'IDENTIFIER', 'STRING', 'VARIABLE', 'COMMENT', 'BLOCK', 'EOL'

# TOKEN
class Token():
    def __init__(self, type: str = '', pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length
        self.next = None

def add_token(node, last):
    while node.next != None:
        node = node.next
    node.next = last

# NODES
class Node():
    def __init__(self, key: str = '', value = None):
        self.key = key
        self.value = value
        self.next = None

def set_node(node, key: str, value):
    while node.next != None:
        node = node.next
        if node.key == key:
            node.value = value
            return
    node.next = Node(key, value)

def get_node(node, key: str):
    while node.next != None:
        node = node.next
        if node.key == key:
            return node.value

def del_node(node, key: str):
    while node != None:
        if node.next.key == key:
            node.next = node.next.next
        node = node.next

# PARSER
def parser(token: Token, text: str):
    func = Node()
    vars = Node()

    set_node(func, 'puts', print)
    set_node(func, 'set', lambda key, value: set_node(vars, key, value))
    set_node(func, 'delete', lambda key: del_node(vars, key))
    set_node(func, 'sum', lambda a, b: float(a) + float(b))
    set_node(func, '=', get_node(func, 'set'))
    set_node(func, '+', get_node(func, 'sum'))

    cmd, args = '', []

    while token.next:
        token = token.next
        value = text[token.pos : token.pos + token.length]
        print("> ", token.type, value)

        if token.type == EOL:
            cmd_return = ''
            cmd_func = get_node(func, cmd)
            if cmd_func:
                cmd_return = cmd_func(*args)
            cmd, args = '', []
            if cmd_return:
                return cmd_return, token
        elif token.type == BLOCK:
            block_return, token = parser(token, text)
            args.append(block_return)
        elif token.type == IDENTIFIER and get_node(func, value):
            cmd = value
        elif token.type == VARIABLE:
            args.append(get_node(vars, value))
        elif token.type == COMMENT: pass
        else:
            args.append(value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r' or ch == ';'

def is_char(ch: str) -> bool:
    return ch != '\t' and ch != ' ' and not is_eol(ch)

def lexer(text: str) -> Token:
    text_length: int = len(text)
    pos: int = 0
    tokens: Token = Token()

    while pos < text_length:
        while not is_char(text[pos]): pos += 1
        token_type: str = ''
        init_pos: int = pos

        if text[pos] == '"' or text[pos] == '\'':
            token_type = STRING
            init_pos = pos + 1
            quote_char: str = text[pos]
            pos += 1
            while text[pos] != quote_char: pos += 1
        elif text[pos] == '(':
            token_type = BLOCK
            pos += 1
        elif text[pos] == ')':
            token_type = EOL
            pos += 1
        elif text[pos] == '$':
            token_type = VARIABLE
            init_pos = pos + 1
            while is_char(text[pos]): pos += 1
        elif text[pos] == '#':
            token_type = COMMENT
            init_pos = pos + 1
            while not is_eol(text[pos]): pos += 1
        else:
            token_type = IDENTIFIER
            while pos < text_length and is_char(text[pos]): pos += 1

        add_token(tokens, Token(token_type, init_pos, pos - init_pos))

        if is_eol(text[pos]):
            add_token(tokens, Token(EOL))

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
