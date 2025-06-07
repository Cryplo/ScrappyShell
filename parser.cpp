#include "parser.h"
#include "builtins.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <fcntl.h>

Node::Node(){}
Node::Node(Node* child1, Node* child2){
            this->child1 = child1;
            this->child2 = child2;
        }
NodeType Node::getNodeType(){return NodeType::BASE;}
Node* Node::getChild1(){return child1;}
Node* Node::getChild2(){return child2;}
void Node::setChild1(Node* child1){this->child1 = child1;}
void Node::setChild2(Node* child2){this->child2 = child2;}

CommandNode::CommandNode(
            std::string cmd,
            std::vector<std::string> args
        ){
            this->cmd = cmd;
            this->args = args;
        }
CommandNode::CommandNode(
            Node* child1,
            Node* child2,
            std::string cmd,
            std::vector<std::string> args
        ):Node(child1, child2){
            this->cmd = cmd;
            this->args = args;
        }
std::string CommandNode::getCommand() {return cmd;}
std::vector<std::string> CommandNode::getArgs() {return args;}
NodeType CommandNode::getNodeType(){return NodeType::COMMAND;}

OperatorNode::OperatorNode(
            std::string opr
        ){
            this->opr = opr;
        }
OperatorNode::OperatorNode(
            Node* child1,
            Node* child2,
            std::string opr
        ):Node(child1, child2){
            this->opr = opr;
        }
std::string OperatorNode::getOperator() {return opr;}
NodeType OperatorNode::getNodeType(){
    return NodeType::OPERATOR;
}

GenericNode::GenericNode(
            std::string str
        ){
            this->str = str;
        }
GenericNode::GenericNode(
            Node* child1,
            Node* child2,
            std::string str
        ):Node(child1, child2){
            this->str = str;
        }
std::string GenericNode::getString() {return str;}
NodeType GenericNode::getNodeType(){
            return NodeType::GENERIC;
        }

PipeCommand::PipeCommand(GenericCommand* cmd1, GenericCommand* cmd2){
    this->cmd1 = cmd1;
    this->cmd2 = cmd2;
}
PipeCommand::execute(){
    //fork, then execute cmd1 and cmd2 and pipe
}

RedirCommand::RedirCommand(GenericCommand* cmd, std::string output){
    this->cmd = cmd;
    this->output = output;
}
RedirCommand::execute(){
    //assume in child process
    dup2(open(output, O_RDWR), 1);
    cmd->execute()
}

std::set<std::string> extcmds = {
    "ls",
    "grep",
    "pwd",
    "echo"
};

std::set<std::string> builtincmds = {
    "cd",
    "quit"
};

//{"|", ">", "<"};
std::map<std::string, std::pair<NodeType, NodeType>> operatorPattern = {
    {"|", std::make_pair(NodeType::COMMAND, NodeType::COMMAND)},
    {">", std::make_pair(NodeType::COMMAND, NodeType::GENERIC)},
    {"<", std::make_pair(NodeType::COMMAND, NodeType::GENERIC)}
};


//FSM for parsing?
void parse(std::vector<Token> tokens, bool* alive){

    //first check for symbols, then separate commands and arguments into nodes
    //when running nodes, do bottom of tree first, then move up
    //ls | grep .h > testing.txt
    //in this example, ls | grep .h is run first (lower), then input is redirected to testing.txt
    //every node has a command and then args grep ["grep", ".h", NULL]

    //Nodes can be either a command or a file or an operator?

    //write separate logic for echo
    //need to add inferencing for generic/command as they may be confused
    ParseState state = ParseState::START;
    std::string cmd = "";
    std::vector<std::string> args;
    std::vector<Node*> nodes;
    for(Token token : tokens){
        if(state == ParseState::START){
            if(extcmds.count(token.value) == 0 && builtincmds.count(token.value) == 0){
                GenericNode* gn = new GenericNode(token.value);
                nodes.push_back(gn);
            }
            else{
                cmd = token.value;
                args.push_back(token.value);
                state = ParseState::ARGS;
            }
        }
        else{
            if(token.type == TokenType::SYMBOL){
                //save this to a node
                // TODO: Currently this has object slicing
                CommandNode* cn = new CommandNode(cmd, args);
                nodes.push_back(cn);
                //clear cmd and args for next time
                args.clear();
                cmd = "";
                //set parse state to check for cmd again
                state = ParseState::START;
                //add in an operator node
                OperatorNode* on = new OperatorNode(token.value);
                nodes.push_back(on);
            }
            else{
                args.push_back(token.value);
            }
        }
    }
    //catch case where command args are not explicitly terminated with a symbol due to end of input
    if(cmd != ""){
        CommandNode* cn = new CommandNode(cmd, args);
        nodes.push_back(cn);
    }

    //precedence: do operators that don't require execution first (i.e setting output file)
    //then execute files (i.e piping) (order matters in piping)
    std::queue<Node*> operatorNodes;
    std::vector<Node*>::iterator nodeIterator;
    for(nodeIterator = nodes.begin(); nodeIterator != nodes.end(); ++nodeIterator){
        if((*nodeIterator)->getNodeType() == NodeType::OPERATOR){
            //logic to check if node pattern matches operators
            OperatorNode* currentOperator = static_cast<OperatorNode*>(*nodeIterator);
            if(operatorPattern.count(currentOperator->getOperator()) == 0 ||
                ((*(nodeIterator-1))->getNodeType() != operatorPattern[currentOperator->getOperator()].first ||
                (*(nodeIterator+1))->getNodeType() != operatorPattern[currentOperator->getOperator()].second)){
                    unknownString("Something");
                    return;
            }
            (*nodeIterator)->setChild1(*(nodeIterator - 1));
            (*nodeIterator)->setChild2(*(nodeIterator + 1));
            operatorNodes.push((*nodeIterator));
        }
    }

    if(operatorNodes.size() == 0){
        if(nodes.size() != 0){
            switch((*nodes.begin())->getNodeType()){
                case NodeType::COMMAND:
                {
                    CommandNode* cn = static_cast<CommandNode*>(*nodes.begin());
                    if(extcmds.count(cn->getCommand()) != 0){
                        int c_pid = fork();
                        //child process
                        if(c_pid == 0){
                            //need to use dup2 instead
                            char* argsArray[cn->getArgs().size() + 1];
                            for(int i = 0; i < cn->getArgs().size(); i++) argsArray[i] = &(cn->getArgs()[i][0]);
                            argsArray[cn->getArgs().size()] = nullptr;
                            execvp(argsArray[0], argsArray);
                            exit(0);
                        }
                        //parent process
                        else{
                            waitpid(c_pid, NULL, 0);
                        }
                    }
                    else if(builtincmds.count(cn->getCommand()) != 0){
                        if(cn->getCommand() == "quit"){
                            quit(alive);
                            return;
                        }
                        else if(cn->getCommand() == "cd"){
                            std::cout << "not implemented yet" << std::endl;
                        }
                        else{
                            unknownCommand(cn->getCommand());
                        }
                    }
                    break;
                }
                case NodeType::OPERATOR:
                {
                    std::cout << "Expected a command, not an operator" << std::endl;
                    break;
                }
                case NodeType::GENERIC:
                {
                    GenericNode* gn = static_cast<GenericNode*>(*nodes.begin());
                    unknownCommand(gn->getString());
                    break;
                }
                default:
                    unknownString("Input");
                    break;
            }
        }
    }
    else{
        while(operatorNodes.size() > 0){
            OperatorNode* currentOperator = static_cast<OperatorNode*>(operatorNodes.front());
            operatorNodes.pop();
            if(currentOperator->getOperator() == ">"){
                CommandNode* cn = static_cast<CommandNode*>(currentOperator->getChild1());
                GenericNode* gn = static_cast<GenericNode*>(currentOperator->getChild2());
                int c_pid = fork();
                //child process
                if(c_pid == 0){
                    //need to use dup2 instead
                    dup2(open(&((gn->getString())[0]), O_RDWR), 1);
                    char* argsArray[cn->getArgs().size() + 1];
                    for(int i = 0; i < cn->getArgs().size(); i++) argsArray[i] = &(cn->getArgs()[i][0]);
                    argsArray[cn->getArgs().size()] = nullptr;
                    execvp(argsArray[0], argsArray);
                    exit(0);
                }
                //parent process
                else{
                    waitpid(c_pid, NULL, 0);
                }
            }
        }
    }
    

    // prevent memory leak
    for(Node* node : nodes){
        delete node;
    }
}

//recursively descent through the tree
/*
void executeNode(Node* node){

}*/