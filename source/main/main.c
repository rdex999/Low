#include <stdio.h>
#include "../fileFuncs/fileFuncs.h"
#include "../lexer/lexer.h"

void printUsage()
{
    printf("low <file.low>\n");
}

int main(int argc, char** argv)
{
    if(argc < 2){
        printf("Incorrect usage.\nCorrect usage: ");
        printUsage();
        return 1; 
    }

    char* content = readFile(argv[1]);
    if(!content){ return 1; }
    puts(content);

    
    free(content);


    return 0;
}