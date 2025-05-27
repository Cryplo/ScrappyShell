#include <iostream>
#include "builtins.h"

void echo(std::string output){
    std::cout << output << std::endl;
}

void quit(bool* alive){
    (*alive) = false;
}

//TODO: Suggest maybe there was a typo?
void unknown(Token token){
    std::cout << "The " << (token.type == TokenType::WORD ? "WORD " : "SYMBOL ") << token.value << " is not recognized." << std::endl;
}