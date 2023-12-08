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
        std::string buffer;
        unsigned int index = 0;
        std::vector<token> tokens;

        inline bool opOpenCloseCheck(size_t idx);
        inline bool addKeyword(const char* keyword, tokenType tok);
        inline char take();
};