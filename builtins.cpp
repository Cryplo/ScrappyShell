#include <iostream>
#include "builtins.h"

void echo(std::string output){
    std::cout << output << std::endl;
}

void quit(bool* alive){
    (*alive) = false;
}

void unknown(std::string str){
    std::cout << str << " is not recognized. Cancelling." << std::endl;
}

//TODO: Suggest maybe there was a typo?
void unknown(Token token){
    std::cout << "The " << (token.type == TokenType::WORD ? "WORD " : "SYMBOL ") << token.value << " is not recognized." << std::endl;
}

void unknownCommand(Token token){
    std::cout << "The command " << token.value << " is not recognized." << std::endl;
}