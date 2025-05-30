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
    CommandNode(
        std::string cmd,
        std::vector<std::string> args
    ){
        this->cmd = cmd;
        this->args = args;
    }
    CommandNode(
        Node* child1,
        Node* child2,
        std::string cmd,
        std::vector<std::string> args
    ){
        Node(child1, child2);
        this->cmd = cmd;
        this->args = args;
    }
    std::string getCommand() return cmd;
    std::vector<std::string> getArgs() return args;
};

struct OperatorNode : Node{
    std::string operator;
    OperatorNode(
        std::string operator
    ){
        this->operator = operator;
    }
    OperatorNode(
        Node* child1,
        Node* child2,
        std::string operator
    ){
        this->child1 = child1;
        this->child2 = child2;
        this->operator = operator;
    }
    std::string getOperator() return operator;
}

//Not CMD / Operator
struct StringNode : Node{
    std::string str;
    StringNode(
        std::string str
    ){
        this->str = str;
    }
    StringNode(
        Node* child1,
        Node* child2,
        std::string str
    ){
        this->child1 = child1;
        this->child2 = child2;
        this->str = str;
    }
    std::string getString() return str;
}

extern std::set<std::string> cmds;

void parse(std::vector<Token> tokens, bool* alive);

#endif