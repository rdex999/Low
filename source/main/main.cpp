#include <fstream>
#include "sstream"
#include "../lexer/lexer.h"
#include "../parse/parse.h"
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
    
    std::vector<token> tokens = lexer(&content).createTokens();
    parse prased(&tokens);

    for(const node::st& st : prased.prog.sts)
    {
        std::cout << "statement of " << (int)st.key.type << "\nwith value of ";
        for(const token& t : st.vals){
            std::cout << t.value << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}