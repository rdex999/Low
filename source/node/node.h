#pragma once
#include <vector>
#include "../token/token.h"

namespace node
{
    // statement
    struct st
    {
        // the statement (int, exit..)
        const token key;

        // the values after the statement.
        // for example if there is 'exit' then 'after' is the number. (exit 5;)
        std::vector<token> vals; 
    };
    

    struct program
    {
        // a vector of statements
        std::vector<st> sts; 
    };
};