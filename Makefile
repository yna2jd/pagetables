LDLIBS =
LDFLAGS = -L. -fsanitize=address
CFLAGS = -fsanitize=address -Og -g -Wall -Werror -std=c11
CC = gcc

all: libmlpt.a

libs: libmlpt.a

clean:
	rm -f *.o *.a *.gch mlpt

libmlpt.a: mlpt.c test.c util.c config.h mlpt.h test.h util.h
	ar rcs $@ $^

mlpt: mlpt.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)

test: test.o mlpt.o util.o main.o

%.o: %.c %.h
	$(CC) $(LDFLAGS) $(CFLAGS) -c $^ $(LDLIBS)

