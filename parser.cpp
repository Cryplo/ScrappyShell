#include "parser.h"

//FSM for parsing?
void parse(std::vector<Token> tokens){

    ParseState state = ParseState::ECHO;

    for(Token token : tokens){
        if(state == ParseState::START){
            if(token.type == TokenType::COMMAND){
                if(token.value == "string"){
                    state = ParseState::ECHO;
                }
            }
        }
        else{
            
        }
    }

}