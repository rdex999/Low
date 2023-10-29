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
    _int,
    add, // +
    sub, // -
    mul, // *
    div, // /
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
