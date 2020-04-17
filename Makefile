CC = g++
CXXFLAGS = -g3 -Wall -Wextra -Wshadow -pedantic -std=c++2a
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lyojimbo -lsodium -lmbedtls -lmbedx509 -lmbedcrypto
INC = -I include
LIB = -L lib
COMMON_SRC_DIR = src/common
CLIENT_SRC_DIR = src/client
SERVER_SRC_DIR = src/server
CLIENT_OBJ_DIR = obj/client
SERVER_OBJ_DIR = obj/server
CLIENT_TARGET = bin/agario_client
SERVER_TARGET = bin/agario_server

CLIENT_SRC_FILES = $(wildcard $(CLIENT_SRC_DIR)/*.cpp $(COMMON_SRC_DIR)/*.cpp)
SERVER_SRC_FILES = $(wildcard $(SERVER_SRC_DIR)/*.cpp $(COMMON_SRC_DIR)/*.cpp)
CLIENT_OBJ_FILES := $(patsubst $(CLIENT_SRC_DIR)/%.cpp, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SRC_FILES))
CLIENT_OBJ_FILES := $(patsubst $(COMMON_SRC_DIR)/%.cpp, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_OBJ_FILES))
SERVER_OBJ_FILES := $(patsubst $(SERVER_SRC_DIR)/%.cpp, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SRC_FILES))
SERVER_OBJ_FILES := $(patsubst $(COMMON_SRC_DIR)/%.cpp, $(SERVER_OBJ_DIR)/%.o, $(SERVER_OBJ_FILES))

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
	rm -rf bin/* obj/* Agario.zip

.PHONY: run

run:
	./bin/agario_client

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
	fd -e cpp -e hpp -t f -x clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4}" -i {}

.PHONY: package

package:
	make
	mkdir Agario
	cp ./bin/agario_client Agario/
	cp -r assets Agario/
	zip -r Agario Agario
	rm -rf Agario
