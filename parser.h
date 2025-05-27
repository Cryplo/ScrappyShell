#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "lexer.h"

enum ParseState{
    START,
    ECHO
};

void parse(std::vector<Token> tokens, bool* alive);

#endif