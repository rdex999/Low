#pragma once
#include <iostream>
#include <vector>
#include <optional>
#include "../token/token.h"

class lexer
{
    public:
        lexer(const std::string* src);

        std::vector<token> createTokens();

    private:
        std::string src;
        unsigned int index = 0;

        inline std::optional<char> peek(int amount = 0);

        inline char take();
};