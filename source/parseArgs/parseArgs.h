#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

class parseArgs
{
    public:
        parseArgs(int argc, char** argv);

        std::vector<std::string>& operator [] (const std::string& val);

    private:
        void printHelp();
        
        std::unordered_map<std::string, std::vector<std::string>> args;
};