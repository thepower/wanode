CC = clang -std=c11 -Wall -Wextra -pedantic -Werror -m64 -O0 -ggdb
CFLAGS = -DTRACE=0 -DDEBUG=1 -DINFO=1 -DWARN=1 -Imsgpack-c/include
LIBS = m

SRCS=$(wildcard *.c)
OBJS=$(SRCS:c=o)

all: wanode

msgpack-c/libmsgpackc.a:
	[ -d msgpack-c ] || git clone https://github.com/msgpack/msgpack-c.git
	(cd msgpack-c; cmake .; make -f CMakeFiles/msgpackc-static.dir/build.make CMakeFiles/msgpackc-static.dir/depend ; make -f CMakeFiles/msgpackc-static.dir/build.make CMakeFiles/msgpackc-static.dir/build)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $(filter %.c,$^) -o $@

wanode: msgpack-c/libmsgpackc.a $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(foreach l,$(LIBS),-l$(l)) msgpack-c/libmsgpackc.a

.PHONY:
clean:
	rm -rf *.o *.a wanode msgpack-c _build
