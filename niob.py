IDF, STR, VAR, CMT, EOL = 'IDF', 'STR', 'VAR', 'CMT', 'EOL'
EXPR_O, EXPR_C = 'EXPR_O', 'EXPR_C'
BLCK_O, BLCK_C = 'BLCK_O', 'BLCK_C'

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
        self.next = None

def set_cmd(command, key: str, value):
    while command.next != None:
        command = command.next
        if command.key == key:
            command.value = value
            return
    command.next = Command(key, value)

def get_cmd(command, key: str):
    while command.next != None:
        command = command.next
        if command.key == key:
            return command.value

class Variable():
    def __init__(self, key: str = '', value: str = ''):
        self.key: str = key
        self.value: str = value
        self.next = None

def set_var(variable, key: str, value: str):
    while variable.next != None:
        variable = variable.next
        if variable.key == key:
            variable.value = value
            return
    variable.next = Variable(key, value)

def get_var(variable, key: str):
    while variable.next != None:
        variable = variable.next
        if variable.key == key:
            return variable.value

commands = Command()
variables = Variable()

def if_cmd(statement: str, block: Token):
    if statement and statement != 'false':
        eval(block)

set_cmd(commands, 'puts', print)
set_cmd(commands, 'set', lambda key, value: set_var(variables, key, value))
set_cmd(commands, 'sum', lambda a, b: float(a) + float(b))
set_cmd(commands, '=', get_cmd(commands, 'set'))
set_cmd(commands, '+', get_cmd(commands, 'sum'))
set_cmd(commands, 'if', if_cmd)

def eval(token: Token) -> Result:
    cmd_key: str = ''
    cmd_args: list = []

    while token.next:
        token = token.next

        if token.type == CMT: pass
        elif token.type == VAR:
            value = get_var(variables, token.value)
            cmd_args.append(value)
        elif token.type == IDF and get_cmd(commands, token.value):
            cmd_key = token.value
        elif token.type == IDF or token.type == STR:
            cmd_args.append(token.value)
        elif token.type == EXPR_O:
            result: Result = eval(token)
            token = result.current_token
            cmd_args.append(result.last_return)
        elif token.type == BLCK_O:
            cmd_args.append(token)
            blocks_length: int = 1
            while token.next and blocks_length > 0:
                token = token.next
                if token.type == BLCK_O: blocks_length += 1
                elif token.type == BLCK_C: blocks_length -= 1
        elif token.type == BLCK_C:
            return Result()
        elif token.type == EOL or token.type == EXPR_C:
            cmd_return: str = ''
            cmd = get_cmd(commands, cmd_key)
            if cmd: cmd_return = cmd(*cmd_args)
            cmd_key = ''
            cmd_args = []
            if cmd_return: return Result(cmd_return, token)

    return Result()

def is_char(ch: str):
    return ch != ' ' and ch != '\t' and \
        ch != ';' and ch != '\n' and \
        ch != ')'

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
            add_token(tokens, BLCK_O)
        elif text[pos] == '}':
            pos += 1
            add_token(tokens, BLCK_C)
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

    return tokens

def main():
    text = """
        # Script example
        message = 'Hello, world!'
        puts $message
        puts ((1 + 2) + 3)
        if false {
            puts "Shouldn't print"
        }
        if true {
            puts "Should print"
        }
    """
    tokens: Token = lexer(text)
    eval(tokens)

if __name__ == '__main__':
    main()
