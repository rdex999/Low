#include "parse.h"

parse::parse(const std::vector<token>* tokens)
{
    this->tokens = tokens;

    for(index=0; index < tokens->size(); index++)
    {
        parseSt(&tokens->at(index));
    }
}

inline void parse::parseSt(const token* t)
{
    node::st st = node::st{.key = *t};
    if(t->type == tokenType::curlyOpen ||
        t->type == tokenType::curlyClose)
    {
        prog.sts.push_back(st);
        return;
    }
    index++;

    for(; index < tokens->size(); index++){
        if(tokens->at(index).type == tokenType::semicolon){
            prog.sts.push_back(st);
            return; 
        }

        st.vals.push_back(tokens->at(index));

        if(tokens->at(index).type == tokenType::curlyOpen ||
            tokens->at(index).type == tokenType::curlyClose)
        {
            prog.sts.push_back(st);
            return;
        }
    }

    std::cerr << "Error, expected smicolon(;)." << std::endl;
    exit(1);
}