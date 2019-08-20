CFLAGS=-std=c11 -g -static -Wall -Werror
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

tocc: $(OBJS)
	$(CC) -o tocc $(OBJS) $(LDFLAGS)

$(OBJS): tocc.h

test: tocc
	./test.sh

clean:
	rm -f tocc *.o *~ tmp*

.PHONY: test clean
