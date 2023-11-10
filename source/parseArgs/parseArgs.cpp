#include "parseArgs.h"

parseArgs::parseArgs(int argc, char **argv)
{
    if(argc < 2){
        std::cerr << "Incorrect usage, must specify at least one input file.\nRun 'low --help' for more info." << std::endl;
        exit(1);
    }

    for(int i=1; i<argc; ++i){
        
        // creating an std::string for comparing
        std::string arg = argv[i];

        if(arg == "--help" || arg == "-h"){
            printHelp();
            exit(0);
        }

        else if(arg == "-o" || arg == "--output"){
            // check there is and argument after the -o 
            if(i+1 < argc && argv[i + 1][0] != '-'){
                args["output"].push_back(argv[i + 1]);
                i++;
            }else{
                std::cerr << "Error, cannot specify \"" << arg << "\" without a value.\nRun 'low --help' for more info." << std::endl;
                exit(1);
            }
        }

        else if(arg == "-f" || arg == "--format"){
            if(i+1 < argc && argv[i+1][0] != '-'){
                args["format"].push_back(argv[i + 1]);
                i++;
            }else{
                std::cerr << "Error, cannot specify \"" << arg << "\" without a value.\nRun 'low --help' for more info." << std::endl;
                exit(1);
            }
        }

        else if(arg == "-t" || arg == "--tempDirName"){
            if(i+1 < argc && argv[i+1][0] != '-'){
                args["tempDirName"].push_back(argv[i + 1]);
                i++;
            }else{
                std::cerr << "Error, cannot specify \"" << arg << "\" without a value.\nRon 'low --help' for more info." << std::endl;
                exit(1);
            }
        }

        else if(arg == "-nS" || arg == "--noStdLib"){
            args["noStdLib"].push_back("1");
        }

        else if(arg.ends_with(".low")){
            args["input"].push_back(arg);
        }

        else if(arg.ends_with(".o") || arg.ends_with(".obj")){
            args["obj"].push_back(arg);
        }

        else if(arg[0] == '-'){
            std::cerr << "Error, unknown argument \"" << arg << "\"." << std::endl;
            exit(1);
        }
    }

    // check if an input file wasnt specified
    if(!args.contains("input") && !args.contains("obj")){
        std::cerr << "Incorrect usage, must specify at least one input file.\nRun 'low --help' for more info." << std::endl;
        exit(1);
    }

    //
    // set defaults if not set allready
    //
    if(args.find("output") == args.end()){
        args["output"].push_back("a.out");
    }

    if(args.find("tempDirName") == args.end()){
        args["tempDirName"].push_back("lowTempBuildDirectory");
    }

    if(args.find("format") == args.end()){
        args["format"].push_back("elf64");
    }

    if(!args.contains("noStdLib")){
        args["noStdLib"].push_back("0");
    }
}

std::vector<std::string>& parseArgs::operator[](const std::string& val)
{
    return args[val];
}

void parseArgs::printHelp()
{
    std::cout << "The \"low\" compiler.\n";
    std::cout << "Usage: low <file.low> -o output\n\n";
    std::cout << "Opetions:\n\t-h --help\t| print this help message.\n\t";
    std::cout << "-o --output\t| the output file name (default a.out)\n\t";
    std::cout << "-f --format\t| the binary format (default elf64) (currently only working for elf64)\n\t";
    std::cout << "-t --tempDirName| the name of the temp directory that is created (and deleted) (default is 'lowTempBuildDirectory)\n\t";
    std::cout << "-nS --noStdLib\t| if specified, wont link with the low standard library. (default is false, (linking with lowStdLib))" << std::endl;
}