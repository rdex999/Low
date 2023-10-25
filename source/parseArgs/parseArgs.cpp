#include "parseArgs.h"

parseArgs::parseArgs(int argc, char **argv)
{
    if(argc < 2){
        std::cerr << "Incorrect usage, must specify input file.\nRun 'low --help' for more info." << std::endl;
        exit(1);
    }

    for(int i=1; i<argc; i++){
        
        // creating an std::string for comparing
        std::string arg = argv[i];

        if(arg == "--help" || arg == "-h"){
            printHelp();
            exit(0);
        }

        else if(arg == "-o" || arg == "--output"){
            // check there is and argument after the -o 
            if(i+1 < argc){
                args["output"] = argv[i + 1];
                i++;
            }else{
                std::cerr << "Error, cannot specify " << arg << " without a value.\nRun 'low --help' for more info." << std::endl;
                exit(1);
            }
        }

        else if(arg.ends_with(".low")){
            args["input"] = arg;
        }

        else if(arg[0] == '-'){
            std::cerr << "Error, unknown argument \"" << arg << "\"." << std::endl;
            exit(1);
        }
    }
    
    //
    // set defaults if not set allready
    //
    if(args.find("output") == args.end()){
        args["output"] = "a.out";
    }
}

std::string parseArgs::operator[](const std::string& val)
{
    if(args.find(val) != args.end()){
        return args[val];
    }

    return "";
}

void parseArgs::printHelp()
{
    std::cout << "The \"low\" compiler.\n";
    std::cout << "Usage: low <file.low> -o output\n\n";
    std::cout << "Opetions:\n\t-h --help | print this help message.\n\t";
    std::cout << "-o --output | the file name (default a.out)\n\t";
    std::cout << "-f --format | the binary format (default elf64)" << std::endl;
}