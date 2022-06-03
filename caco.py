IDENTIFIER, STRING, VARIABLE, EOL = 'IDENTIFIER', 'STRING', 'VARIABLE', 'EOL'

class Environment():
    def func(self, key, cmd):
        setattr(self, key, cmd)

    def sum(self, a, b):
        return float(a) + float(b)

class Variable():
    def __init__(self, key: str = '', value: str = ''):
        self.key = key
        self.value = value
        self.next = None

    def save(self, key: str, value: str):
        current: Variable = self
        while current.next != None:
            current = current.next
            if current.key == key:
                current.value = value
                return
        current.next = Variable(key, value)

    def find(self, key: str):
        current: Variable = self
        while current.next != None:
            current = current.next
            if current.key == key: return current.value

class Token():
    def __init__(self, type: str = '', pos: int = 0, length: int = 0):
        self.type = type
        self.pos = pos
        self.length = length
        self.next = None

    def add(self, last):
        current: Token = self
        while current.next != None: current = current.next
        current.next = last

# PARSER
def parser(token: Token, text: str):
    vars = Variable()

    env = Environment()
    env.func('set', vars.save)
    env.func('=', vars.save)
    env.func('puts', print)
    env.func('+', env.sum)

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
                cmd_return = getattr(env, cmd)(*cmds_args[i])

            cmds, cmds_args, args = [], [], []
        elif token.type == IDENTIFIER and hasattr(env, value):
            if len(cmds) > 0:
                cmds_args.insert(0, args.copy())
                args = []
            cmds.insert(0, value)
        elif token.type == VARIABLE:
            args.append(vars.find(value))
        else:
            args.append(value)

# LEXER
def is_eol(ch: str) -> bool:
    return ch == '\n' or ch == '\r'

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
            tokens.add(Token(STRING, init_pos + 1, pos - init_pos - 1))
        elif text[pos] == '$':
            while is_char(text[pos]): pos += 1
            tokens.add(Token(VARIABLE, init_pos + 1, pos - init_pos - 1))
        else:
            while is_char(text[pos]): pos += 1
            tokens.add(Token(IDENTIFIER, init_pos, pos - init_pos))

        if is_eol(text[pos]) or is_eol(text[pos + 1]):
            tokens.add(Token(EOL))

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
