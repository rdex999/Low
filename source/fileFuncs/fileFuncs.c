#include "fileFuncs.h"

long fileSize(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

char *readFile(char *path)
{
    FILE* file = fopen(path, "r");
    if(!file){
        printf("Could not open file \"%s\".\n", path);
        return NULL;
    }

    long size = fileSize(file);

    // + ! for the null character '\0'
    char* buffer = malloc(size + 1);
    fread(buffer, sizeof(char), size, file);
    buffer[size] = '\0';
    
    fclose(file);

    //buffer[size] = '\0';
    return buffer;
}
