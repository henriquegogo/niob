set count 0
set num 2

while {$count < 765432} {
  while {$num < 765432} {
    set num [expr {$num * 2}]
  }
  set count [expr {$count + 1}]
}

puts $num
