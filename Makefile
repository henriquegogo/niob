BINNAME = niob
LIBNAME = lib$(BINNAME).so

all: lib bin

lib:
	$(CC) -c -fpic -o $(BINNAME).o $(BINNAME).c
	$(CC) -shared -o $(LIBNAME) $(BINNAME).o
	rm $(BINNAME).o

bin:
	$(CC) -o $(BINNAME) main.c $(BINNAME).c

shared: lib
	$(CC) -o $(BINNAME) main.c -L`pwd` -lniob

static:
	$(CC) -static -o $(BINNAME) main.c $(BINNAME).c

clean:
	test -e $(BINNAME) && rm $(BINNAME) || true
	test -e $(LIBNAME) && rm $(LIBNAME) || true

test: bin
	@./$(BINNAME) ./script.nio
	@make clean >> /dev/null

prompt: bin
	@./$(BINNAME)
	@make clean >> /dev/null
