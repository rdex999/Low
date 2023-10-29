#include "parse.h"

parse::parse(const std::vector<token>* tokens)
{
    this->tokens = tokens;

    for(index=0; index < tokens->size(); index++)
    {
        tokenType tType = tokens->at(index).type;
        index++;
        parseSt(tType);
    }
}

inline void parse::parseSt(tokenType stType)
{
    node::st st = node::st{.key = stType};
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