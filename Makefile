LDFLAGS = -lm
CFLAGS = -fsanitize=address -Og -g -Wall -Werror -std=c11
CC = gcc

all: mlpt

mlpt: main.o mlpt.o util.c
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^

mlpt.c: mlpt.h

util.c: util.h

clean:
	rm -f *.o *.a *.gch mlpt