#include <iostream>
#include <string>
#include <utility>
#include "lexer.h"
#include "parser.h"

int main(){

    // INIT shell
    std::string input;
    bool alive = true;

    while(alive){
        std::cout << "ScrappyShell> ";
        std::getline(std::cin, input);
        parse(tokenize(input), (&alive));
        /*parse*/
        /*
        for(std::pair<TokenType, std::string> p : tokenize(input)){
            std::cout << p.second << std::endl;
        }*/
    }

}