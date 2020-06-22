config ?= debug
ifeq ($(config), release)
	OPTS = -O3
else
	OPTS = -g3 -ggdb3 -Wall -Wextra -Wshadow -pedantic -DDEBUG \
				 -fsanitize=address -fno-omit-frame-pointer
endif

CC = g++
CXXFLAGS = $(OPTS) -std=c++14
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf\
					-lspdlog -lfmt\
					-lndn-cxx
INC = -I include
LIB = -L lib
SHARED_SRC_DIR = src/shared
CLIENT_SRC_DIR = src/client
SERVER_SRC_DIR = src/server
CLIENT_OBJ_DIR = obj/client
SERVER_OBJ_DIR = obj/server
CLIENT_TARGET = bin/agario_client
SERVER_TARGET = bin/agario_server

CLIENT_SRC_FILES = $(wildcard $(CLIENT_SRC_DIR)/*.cpp $(SHARED_SRC_DIR)/*.cpp)
SERVER_SRC_FILES = $(wildcard $(SERVER_SRC_DIR)/*.cpp $(SHARED_SRC_DIR)/*.cpp)
CLIENT_OBJ_FILES := $(patsubst $(CLIENT_SRC_DIR)/%.cpp, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SRC_FILES))
CLIENT_OBJ_FILES := $(patsubst $(SHARED_SRC_DIR)/%.cpp, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_OBJ_FILES))
SERVER_OBJ_FILES := $(patsubst $(SERVER_SRC_DIR)/%.cpp, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SRC_FILES))
SERVER_OBJ_FILES := $(patsubst $(SHARED_SRC_DIR)/%.cpp, $(SERVER_OBJ_DIR)/%.o, $(SERVER_OBJ_FILES))

.PHONY: all
all: client server

.PHONY: client
client: $(CLIENT_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJ_FILES)
	@mkdir -p $(shell dirname $(CLIENT_TARGET))
	$(CC) $^ $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -o $@

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) $< $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -c -o $@

.PHONY: server
server: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ_FILES)
	@mkdir -p $(shell dirname $(SERVER_TARGET))
	$(CC) $^ $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -o $@

$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) $< $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -c -o $@

.PHONY: clean

clean:
	rm -rf bin/* obj/* debug/* Agario.zip

.PHONY: mc_client

mc_client:
	@mkdir -p debug
	valgrind --gen-suppressions=all \
		--suppressions=./utils/linux_sdl.sup \
		--leak-check=full \
		--show-leak-kinds=all \
		--log-file=./debug/valgrind-out.log \
		./bin/agario_client

.PHONY: fmt

fmt:
	fd -e cpp -e hpp -t f -x clang-format -style="{BasedOnStyle: llvm}" -i {}

.PHONY: package

package:
	make clean
	make all config=release
	mkdir Agario
	cp ./bin/agario_client Agario/
	cp ./bin/agario_server Agario/
	strip Agario/agario_client
	strip Agario/agario_server
	cp -r assets Agario/
	zip -r Agario Agario
	rm -rf Agario
