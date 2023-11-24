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

        case tokenType::sub:
            if(!(index - 1 >= 0 && tokens->at(index - 1).type >= tokenType::intLit &&
                tokens->at(index - 1).type <= tokenType::floatLit))
            {
                if(index+1 < tokens->size()){
                    std::string nextTokVal = '-' + tokens->at(++index).value;
                    st.vals.push_back(token{.type = tokens->at(index).type, .value = nextTokVal});
                }else{
                    std::cerr << "Error, cannot use minus operator (-) without a value." << std::endl;
                    exit(1);
                }
            }else{
                st.vals.push_back(tokens->at(index));
            }
            break;

        case tokenType::_for:
            st.vals.push_back(token{.type = tokenType::_for});
            prog.sts.push_back(st);
            return;

        default:
            st.vals.push_back(tokens->at(index));
            if(index + 1 < tokens->size()){
                if(tokens->at(index).type == tokenType::_else &&
                    tokens->at(index+1).type == tokenType::curlyOpen)
                {
                    st.vals.push_back(tokens->at(++index));
                    prog.sts.push_back(st);
                    return;
                }else if(tokens->at(index).type == tokenType::_else && tokens->at(index+1).type == tokenType::_if){
                    st.vals.at(st.vals.size() - 1).type = tokenType::elseIf;
                    ++index;
                }
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