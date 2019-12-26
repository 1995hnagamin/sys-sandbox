.PHONY: all clean

TARGET = client server
OBJS = client.o server.o sockbuf.o

CFLAGS = -std=c11 \
	-Wall -Wextra -Wpedantic

all: $(TARGET)

client: client.o sockbuf.o

server: server.o sockbuf.o

clean:
	rm -rf $(TARGET) $(OBJS)
