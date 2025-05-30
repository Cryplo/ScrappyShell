#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <set>

enum TokenType {
    WORD,
    SYMBOL,
};

enum LexState{
    NORMAL,
    STRING
};

struct Token{
    TokenType type;
    std::string value;
};

extern std::set<std::string> symbols;

std::vector<Token> tokenize(std::string input);

void addToken(std::string *currentString, std::vector<Token> *tokens);

#endif