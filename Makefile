CC = gcc
CFLAGS = -Iinclude -Wall -Werror -Wpedantic -Wconversion -Wshadow -MMD -MP
DEBUGFLAGS = -g -O0

BUILD_RELEASE = build/release/main.o build/release/solver.o build/release/generator.o build/release/utils.o
BUILD_DEBUG = build/debug/main.o build/debug/solver.o build/debug/generator.o build/debug/utils.o

all: bin/release/main

bin/release/main: $(BUILD_RELEASE)
	@mkdir -p bin/release
	$(CC) $(BUILD_RELEASE) -o $@

debug: bin/debug/main

bin/debug/main: $(BUILD_DEBUG)
	@mkdir -p bin/debug
	$(CC) $(BUILD_DEBUG) -o $@

build/release/%.o: src/%.c
	@mkdir -p build/release
	$(CC) $(CFLAGS) -c $< -o $@

build/debug/%.o: src/%.c
	@mkdir -p build/debug
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $< -o $@

run: bin/release/main
	./bin/release/main

run-debug: bin/debug/main
	gdb ./bin/debug/main

clean:
	rm -rf bin build

.PHONY: clean run all debug run-debug

-include build/release/*.d
-include build/debug/*.d
