#pragma once
#include <iostream>
#include <vector>
#include <optional>
#include "../token/token.h"

class lexer
{
    public:
        lexer(std::string src);

        std::vector<token> createTokens();

    private:
        std::string src;
        unsigned int index = 0;

        inline bool opOpenCloseCheck(size_t idx);

        inline char take();
};