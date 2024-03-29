#pragma once
#include <iostream>
#include "../node/node.h"

class parse
{
    public:
        parse(const std::vector<token>* tokens);
        
        node::program prog;

    private:
        size_t index = 0;
        const std::vector<token>* tokens;

        inline void setMainStackSize();

        inline void parseSt(const token* t);
        inline uint selectSize(tokenType tok);
};