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
        
        void genExpr(int valsIdx = 0, int offset = -1);
        
        inline void genExit();
        inline void genInt();

};