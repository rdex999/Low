#pragma once
#include <string>

enum class tokenType
{
    _return,
    _exit,
    intLit,
    semicolon,
    ident,
    _int, // int data type
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
    _else,
    elseIf,
    _or,
    _and,
    bEqual, // boolean equal, ==
    bNotEq, // not equal, !=
    bNot, // not, !
    g, // greater, >
    gEq, // greater or equal to, >=
    l, // less, <
    lEq, // less or equal to <=
    quote, // ' '
    dQoute, // " "
    _char, // char data type
    ptr, // ptr keyword, for pointers. (char ptr myString = "string";)
    singleAnd, // &
};

typedef struct token
{
    tokenType type;
    std::string value = "";
} token;
