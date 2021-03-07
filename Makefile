BINNAME = caco
SRC = $(wildcard *.c)
CFLAGS = -std=gnu11

all:
	$(CC) -o $(BINNAME) $(SRC) $(CFLAGS)

run:
	./$(BINNAME)

clean:
	rm $(BINNAME)
