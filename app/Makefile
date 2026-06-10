CC ?= gcc
CFLAGS ?= -Wall -Wextra -Werror -std=c11 -Iinclude
TARGET = diagnostic-daemon

SRCS = src/diagnostic_daemon.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJS)
