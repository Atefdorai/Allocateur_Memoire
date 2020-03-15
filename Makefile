CC?=gcc

# CFLAGS  = -fPIC -Wall -O2 
# LDFLAGS = -shared
# RM = rm -f

all: run 

setup: include/alloc.h
	mkdir -p ./build/lib
	mkdir -p ./build/include
	cp -r include ./build/

lib: setup lib/alloc.c
	$(CC) -shared -fPIC -o build/lib/liballoc.so lib/alloc.c -ldl

prog: main.c setup
	$(CC) -o build/test $<

run: lib prog
	LD_LIBRARY_PATH=./build/lib # ./build/test 
	LD_PRELOAD=./build/lib/liballoc.so ./build/test

clean:
	rm -rf build/*

