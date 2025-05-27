#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>

enum TokenType {
    COMMAND,
    STRING, //If unrecognized, default to string
};

struct Token{
    TokenType type;
    std::string value;
};

extern std::map<std::string, TokenType> recognized_lexemes;

std::vector<Token> tokenize(std::string input);

void addToken(std::string *currentString, std::vector<Token> *tokens);

#endif