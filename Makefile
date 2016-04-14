all:
	gcc -std=c99 lib/file.c lib/iff.c bpconv/bpconv.c -o build/bpconv && ./build/bpconv testinput/input.iff