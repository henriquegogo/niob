IDENTIFIER, STRING, VARIABLE, COMMENT, EOL = \
'IDENTIFIER', 'STRING', 'VARIABLE', 'COMMENT', 'EOL'

class Token():
    def __init__(self, type: str = '', pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length
        self.next = None

class Node():
    def __init__(self, key: str = '', value = None):
        self.key = key
        self.value = value
        self.next = None

# NODES AND LISTS
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

def add_node(node, last):
    while node.next != None:
        node = node.next
    node.next = last

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

    cmds, cmds_args, args = [], [], []

    while token.next:
        token = token.next
        value = text[token.pos : token.pos + token.length]
        print("> ", token.type, value)

        if token.type == EOL:
            cmds_args.insert(0, args.copy())
            cmd_return = None

            for i, cmd in enumerate(cmds):
                if cmd_return:
                    cmds_args[i].append(cmd_return)
                    cmd_return = None
                cmd_return = get_node(func, cmd)(*cmds_args[i])

            cmds, cmds_args, args = [], [], []
        elif token.type == IDENTIFIER and get_node(func, value):
            if len(cmds) > 0:
                cmds_args.insert(0, args.copy())
                args = []
            cmds.insert(0, value)
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
    pos: int = 0
    tokens: Token = Token()

    while pos + 1 < len(text):
        while not is_char(text[pos]): pos += 1
        init_pos: int = pos

        if text[pos] == '"' or text[pos] == '\'':
            quote_char: str = text[pos]
            pos += 1
            while text[pos] != quote_char: pos += 1
            add_node(tokens, Token(STRING, init_pos + 1, pos - init_pos - 1))
        elif text[pos] == '$':
            while is_char(text[pos]): pos += 1
            add_node(tokens, Token(VARIABLE, init_pos + 1, pos - init_pos - 1))
        elif text[pos] == '#':
            while not is_eol(text[pos]): pos += 1
            add_node(tokens, Token(COMMENT, init_pos + 1, pos - init_pos - 1))
        else:
            while is_char(text[pos]): pos += 1
            add_node(tokens, Token(IDENTIFIER, init_pos, pos - init_pos))

        if is_eol(text[pos]) or is_eol(text[pos + 1]):
            add_node(tokens, Token(EOL))

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
