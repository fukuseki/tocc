CFLAGS=-std=c11 -g -static -Wall -Werror
SRCS=codegen.c main.c parse.c tokenize.c
OBJS=$(SRCS:.c=.o)

tocc: $(OBJS)
	$(CC) -o tocc $(OBJS) $(LDFLAGS)

$(OBJS): tocc.h

foo.o: foo.c
	arm-linux-gnueabihf-gcc -c -o foo.o foo.c

test: tocc foo.o
	./test2.sh

clean:
	rm -f tocc *.o *~ tmp*

.PHONY: test clean
