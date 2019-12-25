.PHONY: all clean

TARGET = nc
OBJS = nc.o

CFLAGS = -std=c11 \
	-Wall -Wextra -Wpedantic

all: $(TARGET)

nc: nc.o sockbuf.o

clean:
	rm -rf $(TARGET) $(OBJS)
