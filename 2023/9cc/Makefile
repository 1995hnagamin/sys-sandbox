CFLAGS += -Wall -Wpedantic -Wextra -std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

tmp: tmp.s foo.o
	$(CC) -c tmp.s
	$(CC) foo.o tmp.o -o tmp

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
