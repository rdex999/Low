#ifndef FILEFUNCS_H
#define FILEFUNCS_H
#include <stdio.h>
#include <stdlib.h>

// returns the size of a file in bytes
// @param {FILE*} a pointer to the file
// @return {long int} the size of the file in bytes
long fileSize(FILE* file);

// reads the content of a file into a buffer
// @param {char*} path
// the path to the file
// @returns {char*} buffer
// a heap allocated array of characters.
char* readFile(char* path);

#endif