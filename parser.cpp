#include "parser.h"
#include "builtins.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

Node::Node(){}
Node::Node(Node* child1, Node* child2){
            this->child1 = child1;
            this->child2 = child2;
        }
NodeType Node::getNodeType(){return NodeType::BASE;}

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

std::set<std::string> cmds = {
    "ls",
    "grep",
    "pwd"
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
    ParseState state = ParseState::START;
    std::string cmd = "";
    std::vector<std::string> args;
    std::vector<Node*> nodes;
    for(Token token : tokens){
        if(state == ParseState::START){
            if(cmds.count(token.value) == 0){
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
    //then execute files (i.e piping)

    // prevent memory leak
    for(Node* node : nodes){
        delete node;
    }


    /*
    for(Node* node : nodes){
        switch(node->getNodeType()){
            case NodeType::GENERIC:
                std::cout << static_cast<GenericNode*>(node)->getString() << std::endl;
                break;
            case NodeType::COMMAND:
                std::cout << static_cast<CommandNode*>(node)->getCommand() << std::endl;
                break;
            case NodeType::OPERATOR:
                std::cout << static_cast<OperatorNode*>(node)->getOperator() << std::endl;
                break;
            default:
                std::cout << "this shouldn't ever happen lol" << std::endl;
                break;
        }
    }
    */



    //WORKING FOR COMMANDS
    /*
    for(Node* node : nodes){
        if(node->getNodeType() == NodeType::COMMAND){
            //use static cast because node is for sure CommandNode
            //create child process to execute command
            int c_pid = fork();
            //child process
            if(c_pid == 0){
                char* argsArray[static_cast<CommandNode*>(node)->getArgs().size() + 1];
                for(int i = 0; i < static_cast<CommandNode*>(node)->getArgs().size(); i++) argsArray[i] = &(static_cast<CommandNode*>(node)->getArgs()[i][0]);
                argsArray[static_cast<CommandNode*>(node)->getArgs().size()] = nullptr;
                execvp(argsArray[0], argsArray);
            }
            //parent process
            else{
                waitpid(c_pid, NULL, 0);
            }
        }
    }*/
    
    /*
    

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
                // Is a child process?
                if(c_pid == 0){
                    execvp("ls", new char*[2]{const_cast<char*>("ls"), NULL});
                    exit(0);
                }
                // Is a parent process?
                else{
                    waitpid(c_pid, NULL, 0);
                }
            }
            else if(token.value == "pwd"){
                pid_t c_pid = fork();
                // Is a child process?
                if(c_pid == 0){
                    execvp("pwd", new char* [2]{const_cast<char*>("pwd"), NULL});
                    exit(0);
                }
                // Is a parent process?
                else{
                    waitpid(c_pid, NULL, 0);
                }
            }
            else if(token.value == "grep"){
                pid_t c_pid = fork();
                // Is a child process?
                if(c_pid == 0){
                    execvp("pwd", new char* [2]{const_cast<char*>("pwd"), NULL});
                    exit(0);
                }
                // Is a parent process?
                else{
                    waitpid(c_pid, NULL, 0);
                }
            }
            else if(token.value == "newshelltest"){
                pid_t c_pid = fork();
                // Is a child process?
                if(c_pid == 0){
                    execvp("./shell.exe", new char* [2]{const_cast<char*>("./shell.exe"), NULL});
                    exit(0);
                }
                // Is a parent process?
                else{
                    waitpid(c_pid, NULL, 0);
                }
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
*/
}
