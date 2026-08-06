CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -std=c99 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
TARGET = cdu

SRCS = main.c utils.c fs.c scanner.c tui.c ui.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
