BINNAME = niob
SRC = $(wildcard *.c)
CFLAGS = -std=gnu11

all:
	$(CC) -o $(BINNAME) $(SRC) $(CFLAGS)

debug:
	$(CC) -g -o $(BINNAME) $(SRC) $(CFLAGS)
	gdb $(BINNAME)

run:
	./$(BINNAME)

clean:
	rm $(BINNAME)

test: all run clean
