#include "lexer.h"

std::map<std::string, TokenType> recognized_lexemes = {
    {"echo", COMMAND},
    {"quit", COMMAND}
}; //TODO: Implement directory navigation (cd, ls, etc...)
//TODO: with the repeat command, there may be an issue with repeat quit

std::vector<Token> tokenize(std::string input){

    std::string currentString;
    std::vector<Token> tokens;

    // TODO: Fix issue with multiple spaces
    for(char character : input){
        if(character != ' '){ // TODO: implement all whitespaces
            //A = 65, Z = 90, add 32 to convert to lowercase
            if((int) character >= 65 && (int) character <= 90){
                character = (char) ((int) character + 32); //convert to lowercase
            }
            currentString += character;
        }
        else{
            addToken(&currentString, &tokens);
        }
    }
    //If input doesn't end with space, the last lexeme won't be recorded. Fix this behavior
    if(currentString != ""){
        addToken(&currentString, &tokens);
    }

    return tokens;
}

void addToken(std::string *currentString, std::vector<Token> *tokens){
    if(recognized_lexemes.count(*currentString) != 0){
        (*tokens).push_back({recognized_lexemes[*currentString], *currentString});
    }
    else{
        (*tokens).push_back({TokenType::STRING, *currentString});
    }
    *currentString = "";
}