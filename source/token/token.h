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
    parenOpen, // (
    parenClose, // )
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
