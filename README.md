# Niob
Niob Programming Language

## Goals
- Many 'host languages' implementations
- Easy to learn how to use and extend
- Flexible
- Fun

### Example
```ruby
# Niob is a language for scripting based on TCL and Ruby
set ten 10
puts ((12 + $ten) + 56 )
message = 'Hello, world!'
puts $message
if false { puts "Shouldn't print" }
if true {
    puts "Should print"
    if true { puts "Nested printed" }
    if false { puts "Nested not printed" }
}
def the_end {
    puts 'Global var:' $message
    puts 'END'
}
the_end
```

## License
MIT
