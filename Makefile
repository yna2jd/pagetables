LDLIBS =
LDFLAGS = -L.
CFLAGS = -Og -g -Wall -Werror -std=c11
CC = gcc

libmlpt.a: mlpt.o
	ar rcs $@ $<

all: libmlpt.a

clean:
	rm -f *.o *.a *.gch

mlpt.o: config.h mlpt.h mlpt.c
	$(CC) $(LDFLAGS) $(CFLAGS) -c $^ $(LDLIBS)

.PHONY: all clean