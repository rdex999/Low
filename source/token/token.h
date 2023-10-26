#pragma once
#include <string>

enum class tokenType
{
    _return,
    _exit,
    intLit,
    semicolon,
    ident,
    equal, // =
    _int
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
