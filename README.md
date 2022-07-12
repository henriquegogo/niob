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
set ten 10
puts (2 * ((12 + $ten) + 56 ))
delete ten
message = 'Hello, world!'
puts Message: $ten $message I'm fine.
if false { puts 'Should not print' } else { puts OK }
if (2 > 1) {
    puts 'Should print'
    if true { puts 'Nested printed' }
    if false { puts 'Nested not printed' }
} elseif (1 == 1) { puts 'Else if' }
puts (true ? 'It`s ok' : 'Not ok')
def the_end {
    puts 'Global var:' $message
    puts 'END'
}
the_end
```

## License
MIT
