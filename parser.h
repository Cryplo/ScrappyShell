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
    START, //not actually a good name because it sometimes may be checking for files
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
        Node();
        Node(Node* child1, Node* child2);
        virtual NodeType getNodeType();
        Node* getChild1();
        Node* getChild2();
        void setChild1(Node* child1);
        void setChild2(Node* child2);
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
        );
        CommandNode(
            Node* child1,
            Node* child2,
            std::string cmd,
            std::vector<std::string> args
        );
        std::string getCommand();
        std::vector<std::string> getArgs();
        NodeType getNodeType() override;
};

class OperatorNode : public Node{
    private:
        std::string opr;
    public:
        OperatorNode(
            std::string opr
        );
        OperatorNode(
            Node* child1,
            Node* child2,
            std::string opr
        );
        std::string getOperator();
        NodeType getNodeType() override;
};

//Not CMD / Operator
//Intended mostly for files
class GenericNode : public Node{
    private:
        std::string str;
    public:
        GenericNode(
            std::string str
        );
        GenericNode(
            Node* child1,
            Node* child2,
            std::string str
        );
        std::string getString();
        NodeType getNodeType() override;
};

extern std::set<std::string> cmds;

void parse(std::vector<Token> tokens, bool* alive);

#endif