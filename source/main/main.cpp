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
    bool lowStdLib = args["noStdLib"].at(0) == "1" ? false : true;

    std::string objFiles;
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

        std::vector<token> tokens = lexer(fileStream.str()).createTokens();

        parse prased(&tokens);
        genAsm generatedAsm(&prased.prog, lowStdLib);

        std::filesystem::create_directory(args["tempDirName"][0]);
        std::ofstream outAsm(args["tempDirName"][0] + '/' + filename + ".asm");
        outAsm << generatedAsm.finalAsm.str();
        outAsm.close();

        system(("nasm -o " + args["tempDirName"][0] + '/' + filename + ".o -f" + args["format"][0] + ' ' +
            args["tempDirName"][0] + '/' + filename + ".asm").c_str());

        objFiles += args["tempDirName"][0] + '/' + filename + ".o ";
    }

    if(args.args.contains("obj")){
        for(const std::string& objFile : args["obj"]){
            objFiles += objFile + ' ';
        }
    }

    if(args["noStdLib"].at(0) == "0"){
        objFiles += "lowStdLib/object/lowStdLib.obj ";
    }

    system(("ld -o " + args["output"][0] + " -m " + selectArchArg(args["format"][0]) + ' ' + objFiles).c_str());

    // comment it out for debug 
    //std::filesystem::remove_all(args["tempDirName"][0]);

    return 0;
}