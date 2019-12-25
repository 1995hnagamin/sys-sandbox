.PHONY: all clean

TARGET = client nc
OBJS = client.o nc.o sockbuf.o

CFLAGS = -std=c11 \
	-Wall -Wextra -Wpedantic

all: $(TARGET)

client: client.o sockbuf.o

nc: nc.o sockbuf.o

clean:
	rm -rf $(TARGET) $(OBJS)
