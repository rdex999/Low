#pragma once
#include <string>

enum class tokenType
{
    _return = 1,
    _goto,
    _exit,
    intLit, // 13
    floatLit, // 10.123
    semicolon, // ;
    colon, // :
    ident,
    lable,
    _int, // int data type
    _float, // float data type
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
    bracketOpen, // [
    bracketClose, // ]
    comma, // ,
    _while,
    _for,
    _break, 
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
