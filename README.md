# Niob
Niob Programming Language

## Goals
- Uniform Function Call Syntax inspiration (function order doesn't matter)
- Many 'host languages' implementations
- Easy to learn how to use and extend
- Flexible
- Fun

### Example
```ruby
# Niob is a language for scripting based on TCL and Ruby

set ten 10                    # A sentence is formed by command and arguments.
twenty set 20                 # The order doesn't matter.
message = "Hello, world"      # Symbols can be commands. "=" is alias for "set".
puts $message                 # Variable reference.

puts (2 * (20 + $ten))        # Multiple sentences can be called with "( )".
puts (+ 2 2)                  # Operators are commands too.

delete ten                    # Manage memory manually.

if false {                    # Blocks are considered one argument "{*}".
    puts 'fake'
} elseif (2 == 2) {           # The "elseif" and "else" are arguments of "if"
    puts 'two is two'         # that need to be in the same sentence, so
    puts 'and how are you?'   # avoid to declare in a new line. Use after "{}".

    # Symbols "?", ":" are alias for "if", "else".
    # And order doesn't matter, remember?
    puts (true ? 'ok' : 'fail')   
} (2 > 1) {                   # You can use this syntax as "elseif". Operators
    puts 'two is bigger'      # are commands too.
} else { puts 'the end' }    

def say_my_name {             # Define functions
    puts 'My name is' $name   # There is no scope vars. Everything is global
}

# Function call parameters will be text prepended to defined body
say_my_name (name = 'me')
puts $name                    # Any variable is global
```

## License
MIT
