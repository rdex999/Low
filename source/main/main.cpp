#include <fstream>
#include "sstream"
#include "../lexer/lexer.h"
#include "../parseArgs/parseArgs.h"

int main(int argc, char** argv)
{
    parseArgs args(argc, argv);

    std::ifstream srcFile(args["input"][0]);
    if(!srcFile){
        std::cerr << "Error: could not open file \"" << args["input"][0] << "\"." << std::endl;
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