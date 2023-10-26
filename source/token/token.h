#pragma once
#include <string>

enum class tokenType
{
    _return,
    _exit,
    intLit,
    semicolon
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
