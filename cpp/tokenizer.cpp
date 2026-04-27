#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "helper.hpp" // Generalized helper functions
#include "tokenizer.hpp"

// Global Variables
const std::string section = "Tokenizer";
static const std::unordered_map<std::string, std::string> patterns = {
    {}, 
};

struct token {
    std::string generate()  {
        // if valueExists return token with value
        return value.empty() ? "{'tag': '" + tag + "', 'line': " 
                + std::to_string(lineCount) + ", 'column': " 
                + std::to_string(tokenStart) + "}" 
                : "{'tag': '" + tag + "', 'value': " + value + ", 'line': " 
                + std::to_string(lineCount) + ", 'column': " 
                + std::to_string(tokenStart) + "}";
    }
    int tokenStart = 1, charCount = 1, lineCount = 1;
    std::string tag, value;
};

class tokenizer {
    void strTokenize(const std::string &source_code, std::string::iterator &pos) {
        bool stringEmpty = true;
        currentToken->tokenStart = charCount;
        ++charCount;
        ++pos;
        currentToken->value += '\'';
        while (*pos != '"') {
            if (pos == source_code.cend()) {
                trivialError("Tokenizer", "missing \"", lineCount, charCount);
            }
            stringEmpty = false;
            ++charCount;
            currentToken->value += *pos;     
            ++pos;
        }
        currentToken->value += '\'';
        ++pos;
        if (stringEmpty) {currentToken->value.clear();}
        currentToken->tag = "identifier";
    }

    void keywordTokenize(const std::string &source_code, std::string::iterator &pos) {
        currentToken->tokenStart = charCount;
        switch (*pos) {
            default:
                while(!isspace(*pos) and pos != source_code.cend()) {
                   currentToken->tag += *pos;
                   ++pos;
                   ++charCount;
                }
        }
    }

    int charCount = 1, lineCount = 1;
    std::vector<token*> tokens;
    token *currentToken = new token;

public:
    // Main tokenizer function
    std::string tokenize(std::string source_code) {
        if (source_code.empty()) {
            trivialError("Tokenizer", "tokenize() missing argument");
        }

        std::string::iterator pos = source_code.begin();

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
                    currentToken->tokenStart = charCount;
                    bool decimalCheck = false;
                    while (std::isdigit(*pos) or *pos == '.') {
                        if (*pos == '.' and decimalCheck) {
                            trivialError("Tokenizer", "multiple decimal points", 
                                         lineCount,charCount);
                        } else if (*pos == '.') {
                            decimalCheck = true;
                        }
                        currentToken->value += *pos;
                        ++pos;
                        ++charCount;
                    }
                    currentToken->tag = "number";
                    currentToken->charCount = charCount;
                    break;
                }
                // Single character tokens
                case '+': case '-': case '*': case '/': case '%': case '(': case ')':
                case '{': case '}': case '[': case ']': case ',': case ':': case ';':
                    currentToken->tokenStart = charCount;
                    currentToken->tag += *pos;
                    ++pos;
                    currentToken->charCount = ++charCount;
                    break;
                // Multi character tokens
                case '!': case '=': case '|': case '&': case '<': case '>':
                    currentToken->tokenStart = charCount;
                    currentToken->tag += *pos;
                    ++pos;
                    currentToken->charCount = ++charCount;
                    break;            
                default:
                    #ifdef DEBUG
                        std::cerr << "About to call keywordTokenize()" << std::endl;
                    #endif
                    keywordTokenize(source_code, pos);
            }
            tokens.push_back(currentToken);
            currentToken = new token;
            #ifdef DEBUG
                std::cerr << "[Debug][Tokenizer] end char: " << *pos
                          << " line: "  << lineCount 
                          << std::endl;
            #endif
        }
        currentToken->tag = "None";
        currentToken->lineCount = lineCount;
        currentToken->tokenStart = charCount;
        tokens.push_back(currentToken);
        currentToken = nullptr;
        
        // Take tokens and throw them into a string
        std::string tokenList = "[";
        for (size_t i = 0; i < tokens.size(); ++i){
            i == tokens.size()-1 ?
                tokenList += tokens[i]->generate() :
                tokenList += tokens[i]->generate() + ", ";
            delete(tokens[i]);
        }
        tokenList += "]";

        return tokenList;
    }
};

// Clean caller for tokenize
std::string tokenize(std::string source_code) {
    tokenizer t;
    return t.tokenize(source_code); 
}

