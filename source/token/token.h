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
    addEq, // +=
    subEq, // -=
    mulEq, // *=
    divEq, // /=
    _int,
    add, // +
    sub, // -
    mul, // *
    div, // /
    parenOpen, // (
    parenClose, // )
    curlyOpen, // {
    curlyClose, // }
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
