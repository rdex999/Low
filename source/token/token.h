#pragma once
#include <string>

enum class tokenType
{
    _return,
    _exit,
    intLit,
    semicolon,
    ident,
    _int,
    equal, // =
    addEq, // +=
    subEq, // -=
    mulEq, // *=
    divEq, // /=
    pp, // ++
    mm, // --
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
