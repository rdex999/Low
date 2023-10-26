#include "parse.h"

parse::parse(const std::vector<token>* tokens)
{
    this->tokens = tokens;

    for(index=0; index < tokens->size(); index++)
    {
        switch (tokens->at(index).type)
        {
        case tokenType::_exit:
            index++;
            parseExit();
            break;
        
        default:
            break;
        }
    }
}

inline void parse::parseExit()
{
    node::st st = node::st{.key = token{.type = tokenType::_exit}};
    for(; index < tokens->size(); index++)
    {
        switch (tokens->at(index).type)
        {
        case tokenType::intLit:
            st.vals.push_back(tokens->at(index));
            break;

        case tokenType::semicolon:
            prog.sts.push_back(st);
            return; 
            break;

        default:
            std::cerr << "Error, expected smicolon (;)" << std::endl;
            exit(1);
            break;
        }
    }

    std::cerr << "Error, expected a smicolon (;)" << std::endl;
    exit(1);
}