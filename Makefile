LDLIBS =
LDFLAGS = -L. -fsanitize=address
CFLAGS = -fsanitize=address -Og -g -Wall -std=c11
CC = gcc

libmlpt.a: util.h config.h mlpt.h util.o mlpt.o
	ar rcs $@ util.o mlpt.o
	ranlib $@

clean:
	rm -f *.o *.a *.gch all

all: util.h test.h util.o libmlpt.a test.o main.o
	 $(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ -lmlpt $(LDLIBS)

%.o: %.c config.h mlpt.h
	$(CC) $(LDFLAGS) $(CFLAGS) -c $^ $(LDLIBS)

