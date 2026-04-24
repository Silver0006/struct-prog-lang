#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include "helper.hpp" // Generalized helper functions
#include "tokenizer.hpp"

static const std::unordered_map<std::string, std::string> patterns = {
    
};

// Global Variables
int tokenStart = 1, charCount = 1, lineCount = 1;
std::string tokens, token;

// Helper functions
std::string genToken(const std::string tag, const bool valueExists=false) {
    // if valueExists return token with value
    return valueExists ? "{'tag': '" + tag + "', 'value': " + token + ", 'line': " 
            + std::to_string(lineCount) + ", 'column': " 
            + std::to_string(tokenStart) + "}, " 
            : "{'tag': '" + tag + "', 'line': " 
            + std::to_string(lineCount) + ", 'column': " 
            + std::to_string(tokenStart) + "}, ";
}

void strTokenize(const std::string &source_code, std::string::iterator &pos) {
    token.clear();
    bool stringEmpty = true;
    tokenStart = charCount;
    ++charCount;
    ++pos;
    token += '\'';
    while (*pos != '"') {
        if (pos == source_code.cend()) {
            trivialError("Tokenizer", "missing \"", lineCount, charCount);
        }
        stringEmpty = false;
        ++charCount;
        token += *pos;     
        ++pos;
    }
    token += '\'';
    ++pos;
    if (stringEmpty) {
        token.clear();
    } else {
        token = genToken("identifier", true);
    }
}

void keywordTokenize(const std::string &source_code, std::string::iterator &pos) {
    token.clear();
    tokenStart = charCount;
    while(!isspace(*pos) and pos != source_code.cend()) {
       token += *pos;
       ++pos;
       ++charCount;
    }
    token = genToken(token);
}

// Main tokenizer function
std::string tokenize(std::string source_code) {
    if (source_code.empty()) {
        trivialError("Tokenizer", "tokenize() missing argument");
    }

    std::string::iterator pos = source_code.begin();
    tokens = "[";
    while (pos != source_code.cend()) {
        switch (*pos) {
            // space, tab, return caridge for windows
            case ' ': case '\t': case '\r':
                ++charCount;
                ++pos;
                break;
            // new line             
            case '\n':
                charCount = 1;
                ++lineCount;
                ++pos;
                break;
            // strings
            case '"':
                strTokenize(source_code, pos);
                break;
            // numbers
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9': {
                token.clear();
                tokenStart = charCount;
                bool decimalCheck = false;
                while (std::isdigit(*pos) or *pos == '.') {
                    if (*pos == '.' and decimalCheck) {
                        trivialError("Tokenizer", "multiple decimal points", 
                                     lineCount,charCount);
                    } else if (*pos == '.') {
                        decimalCheck = true;
                    }
                    token += *pos;
                    ++pos;
                    ++charCount;
                }
                token = genToken("number", true);
                break;
            }
            default:
                #ifdef DEBUG
                    std::cerr << "\nAbout to call keywordTokenize()" << std::endl;
                #endif
                keywordTokenize(source_code, pos);
        }
        tokens.append(token);
        token.clear();
        #ifdef DEBUG
            std::cerr << "[Debug][Tokenizer] end char: " << *pos
                      << " line: "  << lineCount 
                      << std::endl;
        #endif
    }
    tokens += "{'tag': 'None', 'line': " + std::to_string(lineCount)
            + ", 'column: " + std::to_string(charCount) + "}]";
    return tokens;
}


