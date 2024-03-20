LDFLAGS = -lm
CFLAGS = -fsanitize=address -Og -g -Wall -Werror -std=c11
CC = gcc

all: mlpt

mlpt: mlpt.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f *.o *.a *.gch mlpt