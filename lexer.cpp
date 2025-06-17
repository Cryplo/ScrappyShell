#include "lexer.h"
#include <iostream>

std::set<std::string> symbols = {"|", ">", "<", ";"};

std::vector<Token> tokenize(std::string input){
    LexState currentState = LexState::NORMAL;
    std::string currentString;
    std::vector<Token> tokens;

    for(char character : input){
        //logic for quoted strings
        if(character == '\'' || character == '\"'){
            if(currentState == LexState::NORMAL) currentState = LexState::QUOTE;
            else currentState = LexState::NORMAL;
        }
        else if((character != ' ' && character != ';') || currentState == LexState::QUOTE){
            //A = 65, Z = 90, add 32 to convert to lowercase
            if((int) character >= 65 && (int) character <= 90){
                character = (char) ((int) character + 32); //convert to lowercase
            }
            currentString += character;
        }
        else{
            if(currentString != "" || character == ';'){
                if(currentString != "") addToken(currentString, &tokens);
                if(character == ';') addToken(";", &tokens);
                currentString = "";
            }
        }
    }
    //If input doesn't end with space, the last lexeme won't be recorded. Fix this behavior
    if(currentString != ""){
        addToken(currentString, &tokens);
    }

    return tokens;
}

//does not replace currentstring with ""
void addToken(std::string currentString, std::vector<Token>* tokens){
    if(symbols.count(currentString) != 0){
        (*tokens).push_back({TokenType::SYMBOL, currentString});
    }
    else{
        (*tokens).push_back({TokenType::WORD, currentString});
    }
}