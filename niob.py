IDENTIFIER, STRING, VARIABLE, COMMENT, BLOCK, EOL = \
'IDENTIFIER', 'STRING', 'VARIABLE', 'COMMENT', 'BLOCK', 'EOL'

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

class Result():
    def __init__(self, content: str = '', token: Token = Token()):
        self.content = content
        self.current_token = token

def add_token(node, last):
    while node.next != None:
        node = node.next
    node.next = last

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

# EVAL
variables: Node = Node()
functions: Node = Node()

def eval(token: Token, text: str):
    set_node(functions, 'puts', print)
    set_node(functions, 'set', lambda key, value: set_node(variables, key, value))
    set_node(functions, 'delete', lambda key: del_node(variables, key))
    set_node(functions, 'sum', lambda a, b: float(a) + float(b))
    set_node(functions, '=', get_node(functions, 'set'))
    set_node(functions, '+', get_node(functions, 'sum'))

    cmd, args = '', []

    while token.next:
        token = token.next
        value = text[token.pos : token.pos + token.length]

        if token.type == EOL:
            cmd_return = ''
            cmd_function = get_node(functions, cmd)
            if cmd_function:
                cmd_return = cmd_function(*args)
            cmd, args = '', []
            if cmd_return:
                return Result(cmd_return, token)
        elif token.type == BLOCK:
            result: Result | None = eval(token, text)
            if result:
                token = result.current_token
                args.append(result.content)
        elif token.type == IDENTIFIER and get_node(functions, value):
            cmd = value
        elif token.type == VARIABLE:
            args.append(get_node(variables, value))
        elif token.type == COMMENT: pass
        else:
            args.append(value)

        #print(">> ", token.type, value)
        #print(".. ", "'" + cmd + "'", args)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_closed(ch: str) -> bool:
    return ch == ';' or ch == ')'

def is_space(ch: str) -> bool:
    return ch == ' ' or ch == '\t'

def is_char(ch: str) -> bool:
    return not is_space(ch) and not is_eol(ch) and not is_closed(ch)

def lexer(text: str) -> Token:
    text_length: int = len(text)
    pos: int = 0
    tokens: Token = Token()

    while pos < text_length:
        while is_space(text[pos]): pos += 1
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
        elif text[pos] == ')':
            token_type = EOL
            pos += 1
        elif text[pos] == '#':
            token_type = COMMENT
            while not is_eol(text[pos]): pos += 1
        elif text[pos] == '$':
            token_type = VARIABLE
            init_pos = pos + 1
            while is_char(text[pos]): pos += 1
        elif is_char(text[pos]):
            token_type = IDENTIFIER
            while is_char(text[pos]): pos += 1
        else:
            token_type = EOL

        add_token(tokens, Token(token_type, init_pos, pos - init_pos))

        if is_eol(text[pos]) or is_closed(text[pos]):
            add_token(tokens, Token(EOL))

        pos += 1

    return tokens

def read_file(filename: str):
    file = open(filename, 'r')
    lines: list = file.readlines()
    return "".join(lines)

def main():
    text = read_file("script.nb")
    tokens = lexer(text)
    eval(tokens, text)

if __name__ == '__main__':
    main()
