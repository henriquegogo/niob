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

script:
	./$(BINNAME) script.nio

clean:
	rm $(BINNAME)

test: all script clean

prompt: all run clean
