IDENTIFIER, STRING, VARIABLE, COMMENT, EOL = 'IDF', 'STR', 'VAR', 'CMT', 'EOL'
EXPRESSION, BLOCK_OPEN, BLOCK_CLOSE = 'EXPR', 'BLCK_O', 'BLCK_C'

class Node():
    def __init__(self, key: str = '', value: str = ''):
        self.key = key
        self.value = value
        self.next = None

class Result():
    def __init__(self, content: str = '', node: Node = Node()):
        self.content = content
        self.current = node

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

# EVAL
variables: Node = Node()
functions: Node = Node()

def ifs(statement: str, block: Node):
    if statement and statement != 'false':
        eval(block)

set_node(functions, 'if', ifs)
set_node(functions, 'puts', print)
set_node(functions, 'set', lambda key, value: set_node(variables, key, value))
set_node(functions, 'delete', lambda key: del_node(variables, key))
set_node(functions, 'sum', lambda a, b: float(a) + float(b))
set_node(functions, '=', get_node(functions, 'set'))
set_node(functions, '+', get_node(functions, 'sum'))

def eval(token: Node):
    cmd, args = '', []

    while token.next:
        token = token.next

        if token.key == EOL:
            cmd_return = ''
            cmd_function = get_node(functions, cmd)
            if cmd_function:
                cmd_return = cmd_function(*args)
            cmd, args = '', []
            if cmd_return:
                return Result(cmd_return, token)
        elif token.key == EXPRESSION:
            result: Result | None = eval(token)
            if result:
                token = result.current
                args.append(result.content)
        elif token.key == BLOCK_OPEN:
            args.append(token)
            blocks_length: int = 1
            while token.next and blocks_length > 0:
                token = token.next
                if token.key == BLOCK_OPEN:
                    blocks_length += 1
                elif token.key == BLOCK_CLOSE:
                    blocks_length -= 1
        elif token.key == BLOCK_CLOSE:
            return Result()
        elif token.key == IDENTIFIER and get_node(functions, token.value):
            cmd = token.value
        elif token.key == VARIABLE:
            args.append(get_node(variables, token.value))
        elif token.key == COMMENT: pass
        else:
            args.append(token.value)

        #print(">> ", token.key, value)
        #print(".. ", "'" + cmd + "'", args)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

def is_closed(ch: str) -> bool:
    return ch == ';' or ch == ')' or ch == '}'

def is_space(ch: str) -> bool:
    return ch == ' ' or ch == '\t'

def is_char(ch: str) -> bool:
    return not is_space(ch) and not is_eol(ch) and not is_closed(ch)

def lexer(text: str) -> Node:
    text_length: int = len(text)
    pos: int = 0
    tokens: Node = Node()

    while pos < text_length:
        while is_space(text[pos]): pos += 1
        init_pos: int = pos

        if text[pos] == '"' or text[pos] == '\'':
            init_pos = pos + 1
            quote_char: str = text[pos]
            pos += 1
            while text[pos] != quote_char: pos += 1
            add_node(tokens, Node(STRING, text[init_pos : pos]))
        elif text[pos] == '(':
            add_node(tokens, Node(EXPRESSION))
        elif text[pos] == ')':
            add_node(tokens, Node(EOL))
            pos += 1
        elif text[pos] == '{':
            add_node(tokens, Node(BLOCK_OPEN))
        elif text[pos] == '}':
            add_node(tokens, Node(EOL))
            add_node(tokens, Node(BLOCK_CLOSE))
        elif text[pos] == '#':
            add_node(tokens, Node(COMMENT))
            while not is_eol(text[pos]): pos += 1
        elif text[pos] == '$':
            init_pos = pos + 1
            while is_char(text[pos]): pos += 1
            add_node(tokens, Node(VARIABLE, text[init_pos : pos]))
        elif is_char(text[pos]):
            while is_char(text[pos]): pos += 1
            add_node(tokens, Node(IDENTIFIER, text[init_pos : pos]))
        else:
            add_node(tokens, Node(EOL))

        if is_eol(text[pos]) or is_closed(text[pos]):
            add_node(tokens, Node(EOL))

        pos += 1

    return tokens

def read_file(filename: str):
    file = open(filename, 'r')
    lines: list = file.readlines()
    return "".join(lines)

def main():
    text = read_file("script.nio")
    tokens = lexer(text)
    eval(tokens)

if __name__ == '__main__':
    main()
