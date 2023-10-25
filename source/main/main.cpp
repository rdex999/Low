#include <fstream>
#include "sstream"
#include "../lexer/lexer.h"

void printUsage()
{
    std::cout << "Usage: low <file.low>" << std::endl;
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        std::cerr << "Incorrect usage, must specify input file.\n";
        printUsage();
        return 1;
    }

    std::ifstream srcFile(argv[1]);
    if(!srcFile){
        std::cerr << "Error: could not open file \"" << argv[1] << "\"." << std::endl;
        exit(1);
    }
    std::stringstream fileStream;
    fileStream << srcFile.rdbuf();
    srcFile.close();
    std::string content = fileStream.str();
    
    for(token t : lexer(&content).createTokens()){
        std::cout << "type: " << (int)t.type << "\ncontent: ";
        if(t.value != ""){
            std::cout << t.value << "\n" << std::endl;
        }else{
            std::cout << "\n" << std::endl;
        }
    }

    return 0;
}