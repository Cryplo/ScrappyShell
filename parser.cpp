#include "parser.h"
#include "builtins.h"
#include <unistd.h>
#include <sys/wait.h>

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
    ParseState state = ParseState::CMD;
    std::string cmd = "";
    std::vector<std::string> args;
    std::vector<Node*> nodes;
    for(Token token : tokens){
        if(state == ParseState::CMD){
            if(cmds.count(token.value) == 0){
                unknown(token);
                break;
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
                CommandNode cn = CommandNode(cmd, args);
                nodes.push_back(&cn);
                //clear cmd and args for next time
                args.clear();
                cmd = "";
                //set parse state to check for cmd again
                state = ParseState::CMD;
            }
            else{
                args.push_back(token.value);
            }
        }
    }
    //write more logic for this
    CommandNode cn = CommandNode(cmd, args);
    nodes.push_back(&cn);

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
    }
    
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
