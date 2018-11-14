CC = clang -std=c11 -Wall -Wextra -pedantic -Werror -m64 -O0 -ggdb
CFLAGS = -DTRACE=0 -DDEBUG=1 -DINFO=1 -DWARN=1 -Imsgpack-c/include -Ilz4/lib
LDFLAGS = -Lmsgpack-c -Llz4/lib
LIBS = m msgpackc lz4

SRCS=$(wildcard *.c)
OBJS=$(SRCS:c=o)

.PHONY: clean git_update

all: wanode

clean:
	$(MAKE) -C msgpack-c clean
	$(MAKE) -C lz4 clean
	rm -rf *.o *.a wanode

git_update:
	git submodule init
	git submodule update

msgpack-c/libmsgpackc.a: git_update
	(cd msgpack-c; cmake .; make -f CMakeFiles/msgpackc-static.dir/build.make CMakeFiles/msgpackc-static.dir/depend ; make -f CMakeFiles/msgpackc-static.dir/build.make CMakeFiles/msgpackc-static.dir/build)

lz4/lib/liblz4.a: git_update
	$(MAKE) -C lz4 lib

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@

wanode: msgpack-c/libmsgpackc.a lz4/lib/liblz4.a $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(foreach l,$(LIBS),-l$(l))

test: test.py wanode
	./test.py

