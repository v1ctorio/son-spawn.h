build: build-main build-benchmark

build-main:
       gcc -Wall -Wextra -ggdb -o build/main.o main.c
run: build
       ./main.o

build-benchmark:
       gcc -Wall -Wextra -o build/fork-no-heap.o benchmarks/fork-no-heap.c
       gcc -Wall -Wextra -o build/fork-1G-heap.o benchmarks/fork-XG-heap.c

benchmark: build-benchmark
       ./build/fork-no-heap.o > benchmarks/fork-no-heap.out.txt
       ./build/fork-1G-heap.o > benchmarks/fork-1G-heap.out.txt
       bat benchmarks/*.out.txt