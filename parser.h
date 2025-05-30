#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <set>
#include "lexer.h"

enum ParseState{
    //START,
    //Echo
    //expect a command
    //expect args
    CMD, //not actually a good name because it sometimes may be checking for files
    ARGS
};

//Binary Tree Node
struct Node{
    Node* child1 = nullptr;
    Node* child2 = nullptr;
    Node(Node* child1, Node* child2){
        this->child1 = child1;
        this->child2 = child2;
    }
}

//One node of the AST contains a cmd and args to run
struct CommandNode : Node{
    std::string cmd;
    std::vector<std::string> args;
    CommandNode(std::string cmd,
                std::vector<std::string> args){
        this->cmd = cmd;
        this->args = args;
    }
    CommandNode(Node* child1,
                Node* child2,
                std::string cmd,
                std::vector<std::string> args){
        Node(child1, child2);
        this->cmd = cmd;
        this->args = args;
    }
};

extern std::set<std::string> cmds;

void parse(std::vector<Token> tokens, bool* alive);

#endif