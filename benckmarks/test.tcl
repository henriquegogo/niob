set count 0
set num 2

while {$count < 987654} {
  while {$num < 987654} {
    set num [expr {$num * 2}]
  }
  set count [expr {$count + 1}]
}

puts $num
