#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include "lexer.h"

void echo(std::string output);

void quit(bool* alive);

void unknownString(std::string str);

void unknownToken(Token token);

void unknownCommand(std::string str);

#endif