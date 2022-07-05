IDF, STR, VAR, CMT, BLK, EOL = 'IDF', 'STR', 'VAR', 'CMT', 'BLK', 'EOL'
EXPR_O, EXPR_C = 'EXPR_O', 'EXPR_C'

class Token():
    def __init__(self, token_type: str = '', token_value: str = ''):
        self.type: str = token_type
        self.value: str = token_value
        self.next = None

def add_token(token, key: str = '', value: str = ''):
    while token.next != None:
        token = token.next
    token.next = Token(key, value)

class Result():
    def __init__(self, content: str = '', token: Token = Token()):
        self.last_return = content
        self.current_token = token

class Command():
    def __init__(self, key: str = '', value = None):
        self.key: str = key
        self.value = value
        self.next: Command | None = None

class Variable():
    def __init__(self, key: str = '', value: str = ''):
        self.key: str = key
        self.value: str = value
        self.next: Variable | None = None

class Env():
    def __init__(self):
        self.commands = Command()
        self.variables = Variable()

        set_cmd(self, 'if', lambda condition, block:
                interpret(block) if condition != 'false' else 0)
        set_cmd(self, 'set', lambda key, value: set_var(self, key, value))
        set_cmd(self, 'sum', lambda a, b: float(a) + float(b))
        set_cmd(self, 'puts', print)
        set_cmd(self, '=', get_cmd(self, 'set'))
        set_cmd(self, '+', get_cmd(self, 'sum'))

def set_cmd(env, key: str, value):
    command = env.commands
    while command.next != None:
        command = command.next
        if command.key == key:
            command.value = value
            return
    command.next = Command(key, value)

def get_cmd(env, key: str):
    command = env.commands
    while command.next != None:
        command = command.next
        if command.key == key: return command.value

def set_var(env, key: str, value: str):
    variable = env.variables
    while variable.next != None:
        variable = variable.next
        if variable.key == key:
            variable.value = value
            return
    variable.next = Variable(key, value)

def get_var(env, key: str):
    variable = env.variables
    while variable.next != None:
        variable = variable.next
        if variable.key == key: return variable.value

def eval(token: Token) -> Result:
    cmd_key: str = ''
    cmd_args: list = []
    env = Env()

    while token.next:
        token = token.next

        if token.type == CMT: pass
        elif token.type == VAR:
            value = get_var(env, token.value)
            cmd_args.append(value)
        elif token.type == IDF and get_cmd(env, token.value):
            cmd_key = token.value
        elif token.type == IDF or token.type == STR:
            cmd_args.append(token.value)
        elif token.type == BLK:
            cmd_args.append(token.value)
        elif token.type == EXPR_O:
            result: Result = eval(token)
            token = result.current_token
            cmd_args.append(result.last_return)
        elif token.type == EOL or token.type == EXPR_C:
            cmd_return: str = ''
            cmd = get_cmd(env, cmd_key)
            if cmd: cmd_return = cmd(*cmd_args)
            cmd_key = ''
            cmd_args = []
            if cmd_return: return Result(cmd_return, token)

    return Result()

def is_char(ch: str):
    return ch != ' ' and ch != '\t' and ch != ';' and ch != '\n' and ch != ')'

def lexer(text: str) -> Token:
    text_length: int = len(text)
    pos: int = 0
    tokens: Token = Token()

    while pos + 1 < text_length:
        while text[pos] == ' ' or text[pos] == '\t':
            if pos + 1 == text_length: break
            pos += 1

        if text[pos] == '\n' or text[pos] == ';':
            pos += 1
            add_token(tokens, EOL)
        elif text[pos] == '(':
            pos += 1
            add_token(tokens, EXPR_O)
        elif text[pos] == ')':
            pos += 1
            add_token(tokens, EXPR_C)
        elif text[pos] == '{':
            pos += 1
            init_pos: int = pos
            blocks_length: int = 1
            while blocks_length > 0:
                if text[pos] == '{': blocks_length += 1
                elif text[pos] == '}': blocks_length -= 1
                pos += 1
            add_token(tokens, BLK, text[init_pos : pos - 1])
        elif text[pos] == '$':
            pos += 1
            init_pos: int = pos
            while is_char(text[pos]): pos += 1
            add_token(tokens, VAR, text[init_pos : pos])
        elif text[pos] == '#':
            init_pos: int = pos
            while text[pos] != '\n': pos += 1
            add_token(tokens, CMT, text[init_pos : pos])
        elif text[pos] == '"' or text[pos] == '\'':
            quote_char: str = text[pos]
            pos += 1
            init_pos: int = pos
            while text[pos] != quote_char: pos += 1
            pos += 1
            add_token(tokens, STR, text[init_pos : pos - 1])
        else:
            init_pos: int = pos
            while is_char(text[pos]): pos += 1
            add_token(tokens, IDF, text[init_pos : pos])

    add_token(tokens, EOL)

    return tokens

def interpret(text: str):
    tokens: Token = lexer(text)
    eval(tokens)

def main():
    text = """
        # Script example
        message = 'Hello, world!'
        puts $message
        puts ((1 + 2) + 3)
        if false { puts "Shouldn't print" }
        if true {
            puts "Should print"
            if true { puts "Nested printed" }
            if false { puts "Nested not printed" }
        }
    """
    interpret(text)

if __name__ == '__main__':
    main()
