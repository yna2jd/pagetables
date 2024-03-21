LDLIBS =
LDFLAGS = -L. -fsanitize=address
CFLAGS = -fsanitize=address -Og -g -Wall -Werror -std=c11
CC = gcc
HEADERS = config.h mlpt.h test.h util.h
OBJS = test.o util.o  mlpt.o

all: libmlpt.a

libs: libmlpt.a

clean:
	rm -f *.o *.a *.gch mlpt test

libmlpt.a: $(OBJS) $(HEADERS)
	ar rcs $@ $^ $(OBJS)
	ranlib $@

test: main.o $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c $(HEADERS)
	$(CC) $(LDFLAGS) $(CFLAGS) -c $^ $(LDLIBS) $(HEADERS)

