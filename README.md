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

set message 'Hello, world'      # The standard way to call functions and set var
set text $message               # Get variable value using $ prefix
message = "What's up?"          # Arguments and functions order doesn't matter

puts $message; puts $text       # Character ';' is considered breakline

delete text                     # Remove variables from memory
puts $text

if true {                       # Statements
  puts "It's all true"

  if false {
    puts "Almost true"
  }
}

ten = 10
calc = (($ten + 5 ) + 26)       # Expressions. '=' and '+' are still functions
puts "Calculation:" $calc
puts "END OF FILE"
```

## License
MIT
