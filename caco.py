IDF, STR, VAR, CMT, EOL = 'IDF', 'STR', 'VAR', 'CMT', 'EOL'
EXPR_O, EXPR_C = 'EXPR_O', 'EXPR_C'
BLCK_O, BLCK_C = 'BLCK_O', 'BLCK_C'

class Script():
    def __init__(self, text: str = ''):
        self.text: str = text + '\n'
        self.length: int = len(self.text)
        self.pos: int = 0
        self.lastreturn: str = ''

class Token():
    def __init__(self, token_type: str = '', token_value: str = ''):
        self.type: str = token_type
        self.value: str = token_value
        self.next = None

def add_token(token, key: str = '', value: str = ''):
    while token.next != None:
        token = token.next
    token.next = Token(key, value)

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

def is_char(ch: str):
    return ch != ' ' and ch != '\t' and \
        ch != ';' and ch != '\n' and \
        ch != ')'

def tokenize(s: Script):
    while s.text[s.pos] == ' ' or s.text[s.pos] == '\t': s.pos += 1

    if s.pos + 1 == s.length:
        return None
    elif s.text[s.pos] == '\n' or s.text[s.pos] == ';':
        s.pos += 1
        return Token(EOL)
    elif s.text[s.pos] == '(':
        s.pos += 1
        return Token(EXPR_O)
    elif s.text[s.pos] == ')':
        s.pos += 1
        return Token(EXPR_C)
    elif s.text[s.pos] == '{':
        s.pos += 1
        return Token(BLCK_O)
    elif s.text[s.pos] == '}':
        s.pos += 1
        return Token(BLCK_C)
    elif s.text[s.pos] == '$':
        s.pos += 1
        init_pos: int = s.pos
        while is_char(s.text[s.pos]): s.pos += 1
        return Token(VAR, s.text[init_pos : s.pos])
    elif s.text[s.pos] == '#':
        init_pos: int = s.pos
        while s.text[s.pos] != '\n': s.pos += 1
        return Token(CMT, s.text[init_pos : s.pos])
    elif s.text[s.pos] == '"' or s.text[s.pos] == '\'':
        quote_char: str = s.text[s.pos]
        s.pos += 1
        init_pos: int = s.pos
        while s.text[s.pos] != quote_char: s.pos += 1
        s.pos += 1
        return Token(STR, s.text[init_pos : s.pos - 1])
    else:
        init_pos: int = s.pos
        while is_char(s.text[s.pos]): s.pos += 1
        return Token(IDF, s.text[init_pos : s.pos])

commands = Command()
variables = Variable()

set_cmd(commands, 'puts', print)
set_cmd(commands, 'set', lambda key, value: set_var(variables, key, value))
set_cmd(commands, 'sum', lambda a, b: float(a) + float(b))
set_cmd(commands, '=', get_cmd(commands, 'set'))
set_cmd(commands, '+', get_cmd(commands, 'sum'))

def eval(script: Script):
    cmd_key: str = ''
    cmd_args: list = []

    while True:
        token = tokenize(script)
        if not token: break
        print(token.__dict__)

        if token.type == CMT: pass
        elif token.type == EXPR_O:
            script = eval(script)
            cmd_args.append(script.lastreturn)
        elif token.type == BLCK_O:
            # Try to avoid to pass a different type of data
            pass
        elif token.type == VAR:
            value = get_var(variables, token.value)
            cmd_args.append(value)
        elif token.type == IDF and get_cmd(commands, token.value):
            cmd_key = token.value
        elif token.type == IDF or token.type == STR:
            cmd_args.append(token.value)
        elif token.type == EOL or token.type == EXPR_C:
            cmd = get_cmd(commands, cmd_key)
            if cmd: script.lastreturn = cmd(*cmd_args)
            cmd_key = ''
            cmd_args = []
            if script.lastreturn: return script

        script.lastreturn = ''

    return script

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
    script = Script(text)
    eval(script)

if __name__ == '__main__':
    main()
