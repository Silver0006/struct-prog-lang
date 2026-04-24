#include <iostream>
#include <string>
#include "tokenizer.hpp"

int main (int argc, char* argv[]) {
    if (argc < 2){
        while(true){
        std::string userInput;
        std::cout << ">>> ";
        std::getline(std::cin, userInput);
        std::cout << tokenize(userInput) << std::endl;
        }
    } else {
        std::cout << tokenize(argv[1]) << std::endl;
    }
    return 0;
}
