all:
	gcc -std=c99 lib/iff.c bpconv/bpconv.c -o build/bpconv && ./build/bpconv