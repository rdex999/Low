#pragma once
#include <sstream>
#include "../node/node.h"

class genAsm
{
    public: 
        genAsm(const node::program* prog);

        std::stringstream outAsm;
    
    private:
        inline void genExit();

        const node::program* prog;

        size_t index = 0;;

        // pushes all registers to the stack
        inline void pusha();

        // pops all registers from the stack
        inline void popa();
};