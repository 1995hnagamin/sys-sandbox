.PHONY: all clean

TARGET = _sh
OBJS = _sh.o \
	cell.o \
	chvec.o

$(TARGET): $(OBJS)

CC = clang

CFLAGS = -std=c11 \
	-Wall -Wextra -Wpedantic

clean:
	rm -rf $(TARGET) $(OBJS)
