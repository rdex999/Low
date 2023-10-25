#pragma once
#include <iostream>
#include <unordered_map>

class parseArgs
{
    public:
        parseArgs(int argc, char** argv);

        std::string operator [] (const std::string& val);

    private:
        void printHelp();
        
        std::unordered_map<std::string, std::string> args;
};