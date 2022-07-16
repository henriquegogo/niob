BINNAME = niob
LIBNAME = lib$(BINNAME).so

all: bin lib

bin:
	$(CC) -o $(BINNAME) main.c $(BINNAME).c

lib:
	$(CC) -c -fpic -o $(BINNAME).o $(BINNAME).c
	$(CC) -shared -o $(LIBNAME) $(BINNAME).o
	rm $(BINNAME).o

clean:
	test -e $(BINNAME) && rm $(BINNAME) || true
	test -e $(LIBNAME) && rm $(LIBNAME) || true

test: bin
	@./$(BINNAME) ./script.nio
	@make clean >> /dev/null

prompt: bin
	@./$(BINNAME)
	@make clean >> /dev/null
