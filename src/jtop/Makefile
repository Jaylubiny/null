CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11 -O2
LIBS = -lncurses

SRCS = main.c cpu.c mem.c ui.c
OBJS = $(SRCS:.c=.o)
TARGET = jtop

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
