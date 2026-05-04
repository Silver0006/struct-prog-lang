#include <cctype>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include "helper.hpp" // Generalized helper functions
#include "tokenizer.hpp"

#ifdef NANOBIND
    #include <nanobind/nanobind.h>
    #include <nanobind/stl/string.h>
#endif

// Global Variables
const std::string section = "Tokenizer";
static const std::unordered_set<std::string> patterns = {
    "true", "false", "null", "function", "return", "extern",
    "if", "else", "while", "for", "break", "continue",
    "print", "import", "exit", "and", "or", "not",
    "assert",
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
        currentToken.tokenStart = charCount;
        ++charCount;
        ++pos;
        currentToken.value += '\'';
        while (*pos != '"') {
            if (pos == source_code.cend()) {
                trivialError("Tokenizer", "missing \"", lineCount, charCount);
            }
            stringEmpty = false;
            ++charCount;
            currentToken.value += *pos;     
            ++pos;
            // Consume double quotes
            if (pos != source_code.cend() and (*pos == '\"' and (pos+1) != source_code.cend() and *(pos + 1) == '\"')){
                currentToken.value += *pos;
                pos += 2;
                charCount += 2;
            }
        }
        currentToken.value += '\'';
        ++pos;
        if (stringEmpty) {currentToken.value.clear();}
        currentToken.tag = "string";
    }

    void keywordTokenize(const std::string &source_code, std::string::iterator &pos) {
        currentToken.tokenStart = charCount;
        while(!isspace(*pos) and pos != source_code.cend()) {
            switch (*pos) {
                // Catch any non-valid characters
                case '"': case '+': case '-': case '*': case '/': case '%': 
                case '(': case ')': case '{': case '}': case '[': case ']': 
                case ',': case ':': case ';': case '!': case '=': case '|':
                case '&': case '<': case '>':
                    break;       
                default:
                    currentToken.value += *pos;
                    ++pos;
                    ++charCount;
                    continue;
            }
            break;
        }
        // Check for keywords
        if (patterns.count(currentToken.value)) {
            if (currentToken.value == "true") {
                currentToken.tag = "boolean";
                currentToken.value = "True";
            } else if (currentToken.value == "false"){
                currentToken.tag = "boolean";
                currentToken.value = "False";
            } else if (currentToken.value == "or") {
                currentToken.tag = "||";
                currentToken.value.clear();
            } else if (currentToken.value == "and") {
                currentToken.tag = "&&";
                currentToken.value.clear();
            } else if (currentToken.value == "not") {
                currentToken.tag = "!";
                currentToken.value.clear();
            } else {
                currentToken.tag = currentToken.value;
                currentToken.value.clear();
            }
        } else {
            currentToken.value = "'" + currentToken.value + "'";
            currentToken.tag = "identifier";
        }
    }

    void multCharTokenize(const std::string &source_code, std::string::iterator &pos) {
        char parentChar = *pos;
        currentToken.tokenStart = charCount;
        currentToken.tag += *pos;
        ++pos;
        currentToken.charCount = ++charCount;
        if (pos == source_code.cend()) {return;}
        switch (*pos) {
            case '=': 
                if (parentChar == '=' or parentChar == '!'
                    or parentChar == '<' or parentChar == '>') {
                    currentToken.tag += *pos;
                    ++pos;
                    currentToken.charCount = ++charCount;
                }
                break;
            case '|': 
                if (parentChar == '|') {
                    currentToken.tag += *pos;
                    ++pos;
                    currentToken.charCount = ++charCount;
                }
                break;
            case '&': 
                if (parentChar == '&') {
                    currentToken.tag += *pos;
                    ++pos;
                    currentToken.charCount = ++charCount;
                }
                break;
            // Comment or Divide
            case '/':
                if (parentChar == '/') {
                    currentToken.tag.clear();
                    currentToken.tag = "comment";
                    while (pos != source_code.cend() and *pos != '\n'){
                        // Escape on newline. Should account for C++ input
                        if (*pos == '\\' 
                            and (pos+1) != source_code.cend() 
                            and *(pos+1) == 'n') {
                            ++lineCount;
                            pos += 2;
                            charCount = 1;
                            break;
                        }
                        ++pos;
                        ++charCount;
                    }
                    emergencySkip = true;
                }
                break;
            default:
                if (parentChar == '&' or parentChar == '|') {
                    trivialError(section, (std::string("expected another ") + parentChar), lineCount, charCount);
                }
        }
    }

    void numTokenize(std::string &source_code, std::string::iterator &pos){
        currentToken.tokenStart = charCount;
        bool decimalCheck = false;
        while ((pos != source_code.cend() and (std::isdigit(*pos) or *pos == '.'))) {
            if (*pos == '.' and decimalCheck) {
                trivialError("Tokenizer", "multiple decimal points", 
                             lineCount,charCount);
            } else if (*pos == '.') {
                decimalCheck = true;
            }
            currentToken.value += *pos;
            ++pos;
            ++charCount;
        }
        currentToken.tag = "number";
        currentToken.charCount = charCount;
    }
    
    int charCount = 1, lineCount = 1;
    std::vector<token> tokens;
    token currentToken;
    bool emergencySkip = false;

public:
    // Main tokenizer function
    std::string tokenize(std::string source_code) {
        if (source_code.empty()) {
            trivialError("Tokenizer", "tokenize() missing argument");
        }

        std::string::iterator pos = source_code.begin();

        while (pos != source_code.cend()) {
            emergencySkip = false;
            switch (*pos) {
                // space, return caridge for windows
                case ' ': case '\r':
                    ++charCount;
                    ++pos;
                    continue;
                    break;
                // tab
                case '\t':
                    charCount += 4 - ((charCount - 1) % 4);
                    ++pos;
                    continue;
                    break;
                // new line
                case '\n':
                    charCount = 1;
                    ++lineCount;
                    ++pos;
                    continue;
                    break;
                // illegal characters
                case '$': case '@':
                    trivialError(section, std::string("Syntax error illegal character: ") + *pos, lineCount, charCount);
                    break;
                // strings
                case '"':
                    strTokenize(source_code, pos);
                    break;
                // numbers
                case '0': case '1': case '2': case '3': case '4': 
                case '5': case '6': case '7': case '8': case '9': {
                    numTokenize(source_code, pos);
                    break;
                }
                // decimal or '.'?
                case '.':
                    currentToken.tokenStart = charCount;
                    currentToken.tag += *pos;
                    if ((pos + 1) != source_code.cend() and std::isdigit(*(pos + 1))) {
                        currentToken.value = "0";
                        numTokenize(source_code, pos);
                        break;
                    }
                    ++pos;
                    currentToken.charCount = ++charCount;
                    break;
                // Single character tokens
                case '+': case '-': case '*': case '%': case '(': case ')':
                case '{': case '}': case '[': case ']': case ',': case ':': case ';':
                    currentToken.tokenStart = charCount;
                    currentToken.tag += *pos;
                    ++pos;
                    currentToken.charCount = ++charCount;
                    break;
                // Multi character tokens
                case '!': case '=': case '|': case '&': case '<': case '>': case '/':
                    multCharTokenize(source_code, pos);            
                    if (emergencySkip) {currentToken.tag.clear(); continue;}
                    break;            
                default:
                    #ifdef DEBUG
                        std::cerr << "About to call keywordTokenize()" << std::endl;
                    #endif
                    keywordTokenize(source_code, pos);
            }
            currentToken.lineCount = lineCount;
            tokens.push_back(currentToken);
            currentToken = token{};
            #ifdef DEBUG
                std::cerr << "[Debug][Tokenizer] end char: " << *pos
                          << " line: "  << lineCount << " pos: " 
                          << charCount << std::endl;
            #endif
        }
        
        // Take tokens and throw them into a string
        std::string tokenList = "[";
        for (size_t i = 0; i < tokens.size(); ++i){
            i == tokens.size()-1 ?
                tokenList += tokens[i].generate() :
                tokenList += tokens[i].generate() + ", ";
        }
        if (tokens.size() > 0) tokenList += ", ";

        // Add end token
        currentToken.tag = "None";
        tokenList += "{'tag': " + currentToken.tag + ", 'line': " 
                + std::to_string(lineCount) + ", 'column': " 
                + std::to_string(charCount) + "}]";

        return tokenList;
    }
};

// Clean caller for tokenize
std::string tokenize(std::string source_code) {
    tokenizer t;
    return t.tokenize(source_code); 
}

#ifdef NANOBIND
NB_MODULE(trivialCPP, m){
    m.def("tokenize", &tokenize);
}
#endif
