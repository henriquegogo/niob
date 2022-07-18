# Niob
Niob Programming Language

## Goals
- Uniform Function Call Syntax inspiration (function order doesn't matter)
- Easy to learn how to use and extend
- Flexible

## Built-in commands
### Actions
- if (alternative: ?)
- set (alternative: =)
- get (alternative: $)
- return (alternative: sentence without command)
- while
- def
- del
- puts
- concat
- len
- item
- size

### Operators
```
+ - * /
&& || == != >= <= > <
```

## Example
```ruby
message = "Hello, world"
puts $message

puts (2 + (2 * 2))

list = one two three
list = $list four
puts ($list size)
```

## Known issues / TODO
- String to number parsing in math
- Comment with unpaired "}" char
- Scoped variables
- Objects

## License
MIT
