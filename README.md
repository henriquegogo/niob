# Niob
Niob Lang

## Example:
```ruby
# Niob is a language for scripting based on TCL and Ruby

set message 'Hello, world'      # The standard way to call functions and set var
set text $message               # Get variable value using $ prefix
message = "What's up?"          # Arguments and functions order doesn't matter

puts $message; puts $text

delete text
puts $text

if false {
  puts 'True story'
}

ten = 10
calc = (($ten + 5 ) + 26)
puts "Calculation:" $calc
puts "END OF FILE"
```

## License
MIT
