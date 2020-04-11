CC = g++
CXXFLAGS = -g3 -Wall -Wextra -Wshadow -pedantic -std=c++2a
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
SRC_DIR = src
OBJ_DIR = obj
TARGET = bin/agario

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf bin/* obj/*

.PHONY: run

run:
	./bin/agario

.PHONY: memcheck

memcheck:
	@mkdir -p debug
	valgrind --gen-suppressions=all \
		--suppressions=./utils/linux_sdl.sup \
		--leak-check=full \
		--show-leak-kinds=all \
		--log-file=./debug/valgrind-out.log \
		./bin/agario

.PHONY: fmt

fmt:
	fd -e cpp -e hpp -t f -x clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i {}
