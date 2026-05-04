test:
	g++ -I . -I tokenizer/ test.cpp tokenizer/tokenizer.cpp helper.cpp -flto -DDEBUG -o testTokenizer
gdb:
	g++ -I . -I tokenizer/ -ggdb test.cpp tokenizer/tokenizer.cpp helper.cpp -flto -DDEBUG -o testTokenizer
cmake:
	cmake -B build --fresh
	cmake --build build --parallel
	cp build/trivialCPP*.so ../trivial
