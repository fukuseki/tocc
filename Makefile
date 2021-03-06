CC=gcc
CFLAGS=-std=c11 -g -Wall -Werror -fsanitize=address -fsanitize=undefined
SRCS=codegen.c main.c tokenize.c parse.c

tocc: $(SRCS)
	$(CC) -o tocc $(SRCS) $(CFLAGS)

foo.o: foo.c
	arm-linux-gnueabihf-gcc -c -o foo.o foo.c

test: tocc foo.o
	./test2.sh

clean:
	rm -f tocc *.o *~ tmp*

.PHONY: test clean
