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
    percentEq, // %=
    pp, // ++
    mm, // --
    add, // +
    sub, // -
    mul, // *
    div, // /
    percent, // %
    parenOpen, // (
    parenClose, // )
    curlyOpen, // {
    curlyClose, // }
    _if,
    _or,
    _and,
    bEqual,
    bNotEq, // not equal, !=
    bNot, // not, !
    g, // greater, >
    gEq, // greater or equal to, >=
    l, // less, <
    lEq, // less or equal to <=
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
