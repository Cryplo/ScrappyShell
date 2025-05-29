
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
            //change this later, testing for now
            else if(token.value == "ls"){
                pid_t c_pid = fork();
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