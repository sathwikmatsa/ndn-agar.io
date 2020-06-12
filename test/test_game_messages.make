test: test_game_messages.cpp ./../src/shared/serialization.cpp
	g++ ./../src/shared/serialization.cpp test_game_messages.cpp -std=c++2a -lndn-cxx -o test
	./test
	rm -f ./test
