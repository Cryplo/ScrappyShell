#include "parser.h"
#include "builtins.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <fcntl.h>

std::set<std::string> extcmds = {
    "ls",
    "grep",
    "pwd",
    "echo",
    "sort",
    "cat"
};

std::set<std::string> builtincmds = {
    "cd",
    "quit"
};

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

void GenericCommand::execute(){
    std::cout << "This should never be reached" << std::endl;
}
GenericCommand::~GenericCommand(){
    
}

SeparateCommand::SeparateCommand(GenericCommand* cmd1, GenericCommand* cmd2){
    this->cmd1 = cmd1;
    this->cmd2 = cmd2;
}
SeparateCommand::~SeparateCommand(){
    delete cmd1;
    delete cmd2;
}
//already in a child process, just fork and run a new one
void SeparateCommand::execute(){
    int c_pid = fork();
    //child process
    if(c_pid == 0){
        cmd1->execute();
    }
    //parent process
    else{
        waitpid(c_pid, NULL, 0);
        cmd2->execute();
    }
}

PipeCommand::PipeCommand(GenericCommand* cmd1, GenericCommand* cmd2){
    this->cmd1 = cmd1;
    this->cmd2 = cmd2;
}
PipeCommand::~PipeCommand(){
    delete cmd1;
    delete cmd2;
    
}
void PipeCommand::execute(){
    int pipefd[2]; //file descriptors of the pipe
    //create a pipe between cmd1 and cmd2
    pipe(pipefd);
    //fork, then execute cmd1 and cmd2
    int c_pid = fork();
    //child process
    //writes to the pipe
    if(c_pid == 0){
        dup2(pipefd[1], 1);
        cmd1->execute();
    }
    //parent process
    //reads from the pipe
    else{
        waitpid(c_pid, NULL, 0);
        close(pipefd[1]);
        dup2(pipefd[0], 0);
        cmd2->execute();
    }
}

ReoutCommand::ReoutCommand(GenericCommand* cmd, std::string output){
    this->cmd = cmd;
    this->output = output;
}
ReoutCommand::~ReoutCommand(){
    delete cmd;
}
void ReoutCommand::execute(){
    //assume in child process
    dup2(open(&(output[0]), O_WRONLY), 1);
    cmd->execute();
}

ReinCommand::ReinCommand(GenericCommand* cmd, std::string input){
    this->cmd = cmd;
    this->input = input;
}
ReinCommand::~ReinCommand(){
    delete cmd;
}
void ReinCommand::execute(){
    dup2(open(&(input[0]), O_RDONLY), 0);
    cmd->execute();
}

ExecCommand::ExecCommand(CommandNode* cn){
    this->cn = cn;
}
ExecCommand::~ExecCommand(){
    
}
void ExecCommand::execute(){
    char* argsArray[cn->getArgs().size() + 1];
    for(int i = 0; i < cn->getArgs().size(); i++) argsArray[i] = &(cn->getArgs()[i][0]);
    argsArray[cn->getArgs().size()] = nullptr;
    //builtin cmd
    if(extcmds.count(cn->getArgs()[0]) == 0){
        if(cn->getArgs()[0] == "cd"){
            cd(cn->getArgs()[1]);
        }
    }
    else{
        execvp(argsArray[0], argsArray);
    }
}

GenericCommand* parseExec(std::vector<Node*>::iterator start, std::vector<Node*>::iterator end, std::vector<Node*> &nodes){
    //there should only be one command here whenever this is called
    if(start == end && (*start)->getNodeType() == NodeType::COMMAND){
        ExecCommand* ec = new ExecCommand(static_cast<CommandNode*>(*start));
        return ec; //kind of scuffed, what if nodes gets deleted? maybe need to malloc
    }
    return nullptr;
}

GenericCommand* parseRedir(std::vector<Node*>::iterator start, std::vector<Node*>::iterator end, std::vector<Node*> &nodes){
    std::vector<Node*>::iterator nodeIterator;
    //parse to look for reout symbol
    //GO IN REVERSE DIRECTION
    for(nodeIterator = end; nodeIterator >= start; nodeIterator--){
        switch((*nodeIterator)->getNodeType()){
            case NodeType::OPERATOR:
            {
                if(static_cast<OperatorNode*>(*nodeIterator)->getOperator() == ">"){
                    ReoutCommand* rc = new ReoutCommand(
                        parseRedir(start, nodeIterator - 1, nodes),
                        static_cast<GenericNode*>(*(nodeIterator+1))->getString() //assume correct grammar and that this is a string
                    );
                    return rc;
                }
                else if(static_cast<OperatorNode*>(*nodeIterator)->getOperator() == "<"){
                    ReinCommand* rc = new ReinCommand(
                        parseRedir(start, nodeIterator - 1, nodes),
                        static_cast<GenericNode*>(*(nodeIterator+1))->getString()
                    );
                    return rc;
                }
            }
            default:
                break;
        }
    }
    return parseExec(start, end, nodes);
}

/*
End is inclusive
*/
GenericCommand* parsePipe(std::vector<Node*>::iterator start, std::vector<Node*>::iterator end, std::vector<Node*> &nodes){
    std::vector<Node*>::iterator nodeIterator;
    //parse to look for pipe symbol
    for(nodeIterator = start; nodeIterator <= end; nodeIterator++){
        switch((*nodeIterator)->getNodeType()){
            case NodeType::OPERATOR:
            {
                if((static_cast<OperatorNode*>(*nodeIterator))->getOperator() == "|"){
                    GenericCommand* pc = new PipeCommand(
                        parseRedir(start, nodeIterator - 1, nodes),
                        parsePipe(nodeIterator + 1, end, nodes)
                    );
                    return pc;
                }
            }
            default:
                break;
        }
    }
    return parseRedir(start, end, nodes);
}

//search for semi-colons
GenericCommand* parseSeparate(std::vector<Node*>::iterator start, std::vector<Node*>::iterator end, std::vector<Node*> &nodes){
    std::vector<Node*>::iterator nodeIterator;
    //parse to look for pipe symbol for a node
    for(nodeIterator = start; nodeIterator <= end; nodeIterator++){
        switch((*nodeIterator)->getNodeType()){
            case NodeType::OPERATOR:
            {
                if((static_cast<OperatorNode*>(*nodeIterator))->getOperator() == ";"){
                    GenericCommand* sc = new SeparateCommand(
                        parsePipe(start, nodeIterator - 1, nodes),
                        parseSeparate(nodeIterator + 1, end, nodes)
                    );
                    return sc;
                }
            }
            default:
                break;
        }
    }
    return parsePipe(start, end, nodes);
}

void parse(std::vector<Token> tokens, bool* alive){
    //first check for symbols, then separate commands and arguments into nodes
    //Nodes can be either a command or a file or an operator?
    //need to add inferencing for generic/command as they may be confused
    ParseState state = ParseState::START;
    std::string cmd = "";
    std::vector<std::string> args;
    std::vector<Node*> nodes;
    for(Token token : tokens){
        if(state == ParseState::START && token.type != TokenType::SYMBOL){
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
    GenericCommand* gc = parseSeparate(nodes.begin(), nodes.end() - 1, nodes);
    int c_pid = fork();
    //child process
    if(c_pid == 0){
        gc->execute();
    }
    //parent process
    else{
        waitpid(c_pid, NULL, 0);
    }

    // prevent memory leak
    for(Node* node : nodes){
        delete node;
    }
    delete gc;
}