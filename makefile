CC = gcc
VERSION = 1.0
FLAGS = -static -D'__BUILD__="$(VERSION)"'
CFLAGS = -O3 -Wall $(FLAGS)

SRCS = mttp.c
OBJS = $(SRCS:.c=.o)
PROG = meowttp

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)

