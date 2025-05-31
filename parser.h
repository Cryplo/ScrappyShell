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

enum NodeType{
    COMMAND,
    OPERATOR,
    GENERIC,
    BASE
};

//Binary Tree Node
class Node{
    private:
        Node* child1 = nullptr;
        Node* child2 = nullptr;
    public:
        Node(){}
        Node(Node* child1, Node* child2){
            this->child1 = child1;
            this->child2 = child2;
        }
        virtual NodeType getNodeType(){
            return NodeType::BASE;
        }
};

//One node of the AST contains a cmd and args to run
//is cmd even needed
class CommandNode : public Node{
    private:
        std::string cmd;
        std::vector<std::string> args;
    public:
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
        std::string getCommand() {return cmd;}
        std::vector<std::string> getArgs() {return args;}
        NodeType getNodeType() override{
            return NodeType::COMMAND;
        }
};

class OperatorNode : public Node{
    private:
        std::string opr;
    public:
        OperatorNode(
            std::string opr
        ){
            this->opr = opr;
        }
        OperatorNode(
            Node* child1,
            Node* child2,
            std::string opr
        ){
            Node(child1, child2);
            this->opr = opr;
        }
        std::string getOperator() {return opr;}
        NodeType getNodeType() override{
            return NodeType::OPERATOR;
        }
};

//Not CMD / Operator
//Intended mostly for files
class GenericNode : public Node{
    private:
        std::string str;
    public:
        GenericNode(
            std::string str
        ){
            this->str = str;
        }
        GenericNode(
            Node* child1,
            Node* child2,
            std::string str
        ){
            Node(child1, child2);
            this->str = str;
        }
        std::string getString() {return str;}
        NodeType getNodeType() override{
            return NodeType::GENERIC;
        }
};

extern std::set<std::string> cmds;

void parse(std::vector<Token> tokens, bool* alive);

#endif