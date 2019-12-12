.PHONY: all clean

TARGET = _sh
OBJS = _sh.o \
	chvec.o

$(TARGET): $(OBJS)

CFLAGS = -std=c11 \
	-Wall -Wextra -Wpedantic

clean:
	rm -rf $(TARGET) $(OBJS)
