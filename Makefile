CFLAGS+=-std=c99 -D_GNU_SOURCE -g -O0 -ldl
DEPS = Makefile

all: user

SRC_USER = src/userspace.c src/dlopen_examples.c

user:
	$(CC) $(SRC_USER) $(CFLAGS) -o userspace

clean:
	rm -rf *.o
