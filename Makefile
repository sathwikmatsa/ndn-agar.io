CC = g++
CXXFLAGS = -g3 -Wall -Wextra -Wshadow -pedantic -std=c++14
LDFLAGS = -lSDL2 -lSDL2_image
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
