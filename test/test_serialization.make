test: test_serialization.cpp ./../src/shared/serialization.cpp
	g++ ./../src/shared/serialization.cpp test_serialization.cpp -std=c++2a -o test
	./test
	rm -f ./test
