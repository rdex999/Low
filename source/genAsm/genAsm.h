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

        // returns the final value in RDX
        void genExpr(int valsIdx = 0);

        // the result is in RDI
        void genMulDiv(int from);
        
        // returns whether the final value is already in RAX
        // true means its in RAX and false means its on the top of the stack  
        // @param {const char*} reg
        // if not pushing to the stack then in which register to store in
        bool genSingle(int idx, const char* reg);

        inline void genExit();
        inline void genInt();

};