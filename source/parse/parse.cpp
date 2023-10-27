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

        case tokenType::_int:
            index++;
            parseInt();
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

inline void parse::parseInt()
{
    node::st st = node::st{.key = token{.type = tokenType::_int}};

    for(; index < tokens->size(); index++){
        switch (tokens->at(index).type)
        {
        case tokenType::semicolon:
            prog.sts.push_back(st);
            return;
            break;

        case tokenType::ident:
            st.vals.push_back(token{.type = tokenType::ident, .value = tokens->at(index).value});
            break;

        case tokenType::equal:
            st.vals.push_back(token{.type = tokenType::equal, .value = tokens->at(index).value}) ;
            break;
        
        case tokenType::intLit:
            st.vals.push_back(token{.type = tokenType::intLit, .value = tokens->at(index).value});
            break;

        default:
            std::cerr << "Error, expected smicolon (;)" << std::endl;
            exit(1);
            break;
        }
    }

    std::cerr << "Error, expected smicolon (;)" << std::endl;
    exit(1);
}
