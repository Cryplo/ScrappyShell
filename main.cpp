#include <iostream>
#include <string>
#include <utility>
#include <thread>
#include <vector>
#include <mutex>
#include "conio.h"
#include "lexer.h"
#include "parser.h"

void keyPressThread(std::vector<std::string> &inputs, std::vector<std::string>::iterator &inputIterator, std::mutex &mtx){
    /*
    while(true){
        switch(getch()){
            case 72:
                mtx.lock();
                if(inputIterator != inputs.begin()) --inputIterator;
                std::cout << "\r" << *inputIterator << std::flush;
                mtx.unlock();
                break;
            case 80:
                mtx.lock();
                if(inputIterator != inputs.end()-1) ++inputIterator;
                std::cout << "\r" << *inputIterator << std::flush;
                mtx.unlock();
                break;
            default:
                break;
        }
    }*/
}

int main(){
    // INIT shell
    std::string input;
    bool alive = true;

    std::vector<std::string> inputs;
    inputs.push_back("dsfaw");
    //use a mutex to prevent keypressthread from altering
    std::vector<std::string>::iterator inputIterator;
    std::mutex mtx;
    //Can probably use non-blocking input to detect keypresses, but I want to implement multithreading
    //std::thread t(keyPressThread, std::ref(inputs), std::ref(inputIterator), std::ref(mtx));
    //t.detach();
    while(alive){
        std::cout << "ScrappyShell> ";
        std::getline(std::cin, input);
        parse(tokenize(input), (&alive));
        inputs.push_back(input);
        mtx.lock();
        inputIterator = inputs.end() - 1;
        mtx.unlock();
    }

}