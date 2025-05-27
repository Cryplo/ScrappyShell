#include "parser.h"
#include "builtins.h"

//FSM for parsing?
void parse(std::vector<Token> tokens){

    ParseState state = ParseState::START;

    for(Token token : tokens){
        if(state == ParseState::START){
            if(token.type == TokenType::WORD){
                if(token.value == "echo"){
                    state = ParseState::ECHO;
                }
            }
        }
        else{
            switch(state){
                case ParseState::ECHO:
                    echo(token.value);
                    break;
            }
        }
    }

}