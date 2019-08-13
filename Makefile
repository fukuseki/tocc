CFLAGS=-std=c11 -g -static

tocc: tocc.c

test: tocc
	./test.sh

clean:
	rm -f tocc *.o *~ tmp*
