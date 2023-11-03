#pragma once
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "../node/node.h"

class genAsm
{
    public: 
        genAsm(const node::program* prog);

        std::stringstream outAsm;
    
    private:

        struct var
        {
            size_t stackLoc;
            int size; // in bytes (int = 4, char = 1..)
        };
        

        std::unordered_map<std::string, var> vars;

        const node::program* prog;

        size_t index = 0;
        size_t stackLoc = 0;
        
        void push(const char* reg, int size, const char* word = "");
        void pop(const char* reg, int size, const char* word = "");

        std::string selectReg(const char* reg, int size);

        // the final value is in RDX
        // @returns {int} the index of i. used when there are parenthesis
        int genExpr(int valsIdx = 0);

        // the result is in RAX
        // @returns {int} the index
        int genMulDiv(int from);
        
        // @returns {int} the index 
        // @param {const char*} reg
        int genSingle(int idx, const char* reg);

        inline void genExit();
        inline void genInt();
        inline void genUpdateIdent();
};