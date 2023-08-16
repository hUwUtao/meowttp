CC = gcc
VERSION = 1.1
FLAGS = -static -s -D'__BUILD__="$(VERSION)"'
CFLAGS = -O3 -Wall $(FLAGS)

SRCS = mttp.c utils.c
OBJS = $(SRCS:.c=.o)
PROG = meowttp

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)

