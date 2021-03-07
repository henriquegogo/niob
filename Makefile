ALL:
	gcc caco.c -o caco.x86_64

run:
	./caco.x86_64

clean:
	rm caco.x86_64
