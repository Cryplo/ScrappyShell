#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include "lexer.h"

void echo(std::string output);

void quit(bool* alive);

void unknown(Token token);

void unknown(std::string str);

void unknownCommand(Token token);

#endif