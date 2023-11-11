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
    node::st st;

    if(t->type == tokenType::curlyOpen ||
        t->type == tokenType::curlyClose)
    {
        st.vals.push_back(*t);
        prog.sts.push_back(st);
        return;
    }

    for(; index < tokens->size(); ++index){
        if(tokens->at(index).type == tokenType::semicolon){
            prog.sts.push_back(st);
            return; 
        }

        switch (tokens->at(index).type)
        {
        case tokenType::_int:
        case tokenType::_char:
            st.vals.push_back(tokens->at(index));
            if(index + 1 < tokens->size() && tokens->at(index + 1).type == tokenType::mul){
                st.vals.push_back(token{.type = tokenType::ptr});
                ++index;
            }
            break;

        default:
            st.vals.push_back(tokens->at(index));
            if(index+1 < tokens->size() && tokens->at(index).type == tokenType::_else &&
                tokens->at(index+1).type == tokenType::curlyOpen)
            {
                st.vals.push_back(tokens->at(++index));
                prog.sts.push_back(st);
                return;
            }
            break;
        }


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