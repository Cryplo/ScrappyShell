#include <iostream>
#include "builtins.h"

void echo(std::string output){
    std::cout << output << std::endl;
}

void quit(bool* alive){
    (*alive) = false;
}

void unknownString(std::string str){
    std::cout << str << " is not recognized. Cancelling." << std::endl;
}

//TODO: Suggest maybe there was a typo?
void unknownToken(Token token){
    std::cout << "The " << (token.type == TokenType::WORD ? "WORD " : "SYMBOL ") << token.value << " is not recognized." << std::endl;
}

void unknownCommand(std::string str){
    std::cout << "The command " << str << " is not recognized." << std::endl;
}