#pragma once
#include <vector>
#include "../token/token.h"

namespace node
{
    // statement
    struct st
    {
        // index 0 is the key 
        std::vector<token> vals; 
    };
    

    struct program
    {
        // a vector of statements
        std::vector<st> sts;

        // the stack usage in main
        uint mainStackSize = 16;
    };
};