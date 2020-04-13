CC = g++
CXXFLAGS = -g3 -Wall -Wextra -Wshadow -pedantic -std=c++2a
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lyojimbo -lsodium -lmbedtls -lmbedx509 -lmbedcrypto
INC = -I include
LIB = -L lib
CLIENT_SRC_DIR = src/client
SERVER_SRC_DIR = src/server
CLIENT_OBJ_DIR = obj/client
SERVER_OBJ_DIR = obj/server
CLIENT_TARGET = bin/agario_client
SERVER_TARGET = bin/agario_server

CLIENT_SRC_FILES = $(wildcard $(CLIENT_SRC_DIR)/*.cpp)
SERVER_SRC_FILES = $(wildcard $(SERVER_SRC_DIR)/*.cpp)
CLIENT_OBJ_FILES = $(patsubst $(CLIENT_SRC_DIR)/%.cpp, $(CLIENT_OBJ_DIR)/%.o, $(CLIENT_SRC_FILES))
SERVER_OBJ_FILES = $(patsubst $(SERVER_SRC_DIR)/%.cpp, $(SERVER_OBJ_DIR)/%.o, $(SERVER_SRC_FILES))

client: $(CLIENT_OBJ_FILES)
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -o $(CLIENT_TARGET) $^

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -c -o $@ $<

server: $(SERVER_OBJ_FILES)
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -o $(SERVER_TARGET) $^

$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) $(INC) $(LIB) $(LDFLAGS) -c -o $@ $<

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
