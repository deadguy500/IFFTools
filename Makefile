all:
	gcc lib/iff.c bpconv/bpconv.c -o build/bpconv && ./build/bpconv