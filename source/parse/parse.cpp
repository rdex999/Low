#include "parse.h"

parse::parse(const std::vector<token>* tokens)
{
    this->tokens = tokens;

    for(index=0; index < tokens->size(); index++)
    {
        parseSt(&tokens->at(index));
    }

    setMainStackSize();
}

inline void parse::setMainStackSize()
{
    for(int idx=0; idx < prog.sts.size(); ++idx){
        if(prog.sts.at(idx).vals.size() > 0){
            if(1 < prog.sts.at(idx).vals.size() && prog.sts.at(idx).vals.at(0).type >= tokenType::_int && prog.sts.at(idx).vals.at(0).type <= tokenType::_char){
                if(prog.sts.at(idx).vals.at(1).type == tokenType::ptr){
                    if(3 < prog.sts.at(idx).vals.size() && prog.sts.at(idx).vals.at(2).type == tokenType::bracketOpen){
                        prog.mainStackSize += 8 + 8 * std::stoi(prog.sts.at(idx).vals.at(3).value);
                        continue;
                    }else{
                        prog.mainStackSize += 8;
                        continue;
                    }
                }else if(2 < prog.sts.at(idx).vals.size() && prog.sts.at(idx).vals.at(1).type == tokenType::bracketOpen){
                    prog.mainStackSize += 8 + selectSize(prog.sts.at(idx).vals.at(0).type) * std::stoi(prog.sts.at(idx).vals.at(2).value);
                    continue;
                }
            }
            switch (prog.sts.at(idx).vals.at(0).type)
            {
            case tokenType::_int:
            case tokenType::_float:
                prog.mainStackSize += 4; 
                break;

            case tokenType::_char:
                ++prog.mainStackSize;
                break;

            default:
                break;
            }
        }
    }
}

inline uint parse::selectSize(tokenType tok)
{
    switch (tok)
    {
    case tokenType::_float:
    case tokenType::_int:
        return 4;

    case tokenType::_char:
        return 1;

    default:
        std::cerr << "PROGRAM ERROR, not valid token for selectSize."  << std::endl;
        exit(1);
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
        case tokenType::_float:
            st.vals.push_back(tokens->at(index));
            if(index + 1 < tokens->size() && tokens->at(index + 1).type == tokenType::mul){
                st.vals.push_back(token{.type = tokenType::ptr});
                ++index;
            }
            break;

        case tokenType::sub:
            if(index - 1 >= 0 && (tokens->at(index - 1).type < tokenType::intLit ||
                tokens->at(index - 1).type > tokenType::floatLit) && tokens->at(index - 1).type != tokenType::ident)
            {
                if(index+1 < tokens->size()){
                    if(tokens->at(index+1).type != tokenType::ident){
                        std::string nextTokVal = '-' + tokens->at(++index).value;
                        st.vals.push_back(token{.type = tokens->at(index).type, .value = nextTokVal});
                    }
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

        case tokenType::colon:
            if(index - 1 >= 0 && tokens->at(index-1).type == tokenType::ident){
                st.vals.at(0).type = tokenType::lable; 
                prog.sts.push_back(st);
                return;
            }
            break;

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