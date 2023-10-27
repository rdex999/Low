#include <fstream>
#include <sstream>
#include <filesystem>
#include "../lexer/lexer.h"
#include "../parse/parse.h"
#include "../genAsm/genAsm.h"
#include "../parseArgs/parseArgs.h"

// returns the architecture for ld
// for example if the argument is --f elf32 then elf_i386
inline std::string selectArchArg(const std::string& str)
{
    if(str == "elf64"){
        return "elf_x86_64";
    }else if(str == "elf32"){
        return "elf_i386";
    }else{
        return "";
    }
}

int main(int argc, char** argv)
{
    parseArgs args(argc, argv);

    std::string files;
    for(const std::string& filename : args["input"])
    {
        std::ifstream srcFile(filename);
        if(!srcFile){
            std::cerr << "Error: could not open file \"" << filename << "\"." << std::endl;
            exit(1);
        }
        std::stringstream fileStream;
        fileStream << srcFile.rdbuf();
        srcFile.close();
        std::string content = fileStream.str();

        std::vector<token> tokens = lexer(&content).createTokens();
        //for(const token& t : tokens) {
        //    std::cout << "Token type: " << (int)t.type << "\nvalue: " << t.value << '\n' << std::endl;
        //}
        
        parse prased(&tokens);
        genAsm generatedAsm(&prased.prog);

        if(!std::filesystem::create_directory(args["tempDirName"][0])){
            std::cerr << "Error, failed to create temp build directory.\nMade sure there is no directory called 'lowTempBuildDirectory'"
                << std::endl;

            return 1;
        }
        
        std::ofstream outAsm(args["tempDirName"][0] + '/' + filename + ".asm");
        outAsm << generatedAsm.outAsm.str();
        outAsm.close();

        system(("nasm -o " + args["tempDirName"][0] + '/' + filename + ".o -f" + args["format"][0] + ' ' +
            args["tempDirName"][0] + '/' + filename + ".asm")
            .c_str());

        files += args["tempDirName"][0] + '/' + filename + ".o";
    }

    system(("ld -o " + args["output"][0] + " -m " + selectArchArg(args["format"][0]) + ' ' + files).c_str());

    // comment it out for debug 
    //std::filesystem::remove_all(args["tempDirName"][0]);

    return 0;
}