#pragma once
#include <vector>
#include "../token/token.h"

namespace node
{
    // statement
    struct st
    {
        // the values after the statement.
        // index 0 is the key 
        std::vector<token> vals; 
    };
    

    struct program
    {
        // a vector of statements
        std::vector<st> sts; 
    };
};