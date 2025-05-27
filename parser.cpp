#include "parser.h"
#include "builtins.h"

//FSM for parsing?
void parse(std::vector<Token> tokens, bool* alive){

    ParseState state = ParseState::START;

    for(Token token : tokens){
        if(state == ParseState::START){
            //Could rewrite to a map (token.value : func), then count to trigger unknown
            //Would help builtin func unknown to suggest alternatives
            if(token.value == "echo"){
                state = ParseState::ECHO;
            }
            else if(token.value == "quit"){
                quit(alive);
            }
            else{
                unknown(token);
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