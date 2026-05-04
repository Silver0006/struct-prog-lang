#include <iostream>
#include <stdexcept>
#include <string>
#include "helper.hpp"

void trivialError(const std::string section, const std::string reason) {
    std::cerr << "[" << section << "] " << reason << std::endl;
    throw std::runtime_error("[" + section + "] " + reason);
}
void trivialError(const std::string section, const std::string reason, const int line, const int column) {
    std::cerr << "[" << section << "] " << reason << " at line: " << std::to_string(line) 
              << " character: " << std::to_string(column) << std::endl;
    throw std::runtime_error("[" + section + "] " + reason 
                             + " at line: " + std::to_string(line) 
                             + " character: " + std::to_string(column));
}
