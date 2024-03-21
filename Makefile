LDLIBS =
LDFLAGS = -L. -fsanitize=address
CFLAGS = -fsanitize=address -Og -g -Wall -Werror -std=c11
CC = gcc
HEADERS = config.h mlpt.h test.h util.h

all: libmlpt.a

libs: libmlpt.a

clean:
	rm -f *.o *.a *.gch mlpt test

libmlpt.a: test.c util.c mlpt.c mlpt_txt.c
	ar rcs $@ $^ $(HEADERS)

test: main.o test.o util.o  mlpt.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) -c $^ $(LDLIBS) $(HEADERS)

