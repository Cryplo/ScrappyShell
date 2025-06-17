#include "builtins.h"
#include <unistd.h>

void quit(bool* alive){
    (*alive) = false;
}

void cd(std::string path){
    chdir(&(path[0]));
}