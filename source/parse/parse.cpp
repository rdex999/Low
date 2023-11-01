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
    index++;
    for(; index < tokens->size(); index++){
        if(tokens->at(index).type == tokenType::semicolon){
            prog.sts.push_back(st);
            return; 
        }

        st.vals.push_back(tokens->at(index));
    }

    std::cerr << "Error, expected smicolon(;)." << std::endl;
    exit(1);
}