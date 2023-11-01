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
        };
        

        std::unordered_map<std::string, var> vars;

        const node::program* prog;

        size_t index = 0;
        size_t stackLoc = 0;
        
        void push(const char* reg);
        void pop(const char* reg);

        // the final value is in RDX
        // @returns {int} the index of i. used when there are parenthesis
        int genExpr(int valsIdx = 0);

        // the result is in RAX
        // @returns {int} the index
        int genMulDiv(int from);
        
        // @returns {bool} true: result in reg false: result on stack 2
        // @param {const char*} reg
        // if not pushing to the stack then in which register to store in
        bool genSingle(int idx, const char* reg);

        inline void genExit();
        inline void genInt();
        inline void genUpdateIdent();
};