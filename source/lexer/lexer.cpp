#include "lexer.h"

lexer::lexer(std::string src)
{
    this->src = std::move(src);
    index = 0;
}

std::vector<token> lexer::createTokens()
{
    std::string buffer;
    std::vector<token> tokens;
    for(index = 0; index < src.size();) 
    {
        if(std::isalpha(src[index])){
            while(index < src.size() && std::isalnum(src[index])){
                buffer += take();
            }

            // keyword detection
            if(buffer == "exit"){
                tokens.push_back(token{.type = tokenType::_exit});
            }

            else if(buffer == "int"){
                tokens.push_back(token{.type = tokenType::_int});
            }

            else if(buffer == "if"){
                tokens.push_back(token{.type = tokenType::_if});
            }

            else if(buffer == "and"){
                tokens.push_back(token{.type = tokenType::_and});
            }

            else if(buffer == "or"){
                tokens.push_back(token{.type = tokenType::_or});
            }

            else if(buffer == "ptr"){
                tokens.push_back(token{.type = tokenType::ptr});
            }

            else if(buffer == "char"){
                tokens.push_back(token{.type = tokenType::_char});
            }

            else{
                tokens.push_back(token{.type = tokenType::ident, .value = buffer});
            }
            buffer.clear();
            continue;

        }else if(std::isdigit(src[index])){
            while(index < src.size() && std::isdigit(src[index])){
                buffer += take();
            }
            tokens.push_back(token{.type = tokenType::intLit, .value = buffer});
            buffer.clear();
            continue;
        }

        else if(std::isspace(src[index])){
            take();
            continue;
        
        }else if(src[index] == ';'){
            take();
            tokens.push_back(token{.type = tokenType::semicolon});
            continue;
        }

        else if(src[index] == '\''){
            if(src.size() > index + 2 && src[index + 2] == '\''){
                buffer += take();
                buffer += take();
                buffer += take();
                tokens.push_back(token{.type = tokenType::quote, .value = buffer});
                buffer.clear();
                continue;
            }else{
                std::cerr << "Error, cannot use single quote ('') on multible characters.\nSingle quotes are for one character."
                    << std::endl;
                
                exit(1);
            }
        }

        else if(src[index] == '"'){
            while(index < src.size()){
                buffer += take();
                if(index < src.size()){
                    if(src[index] == '"'){
                        buffer += take();
                        break; 
                    }
                }else{
                    std::cerr << "Error, forgot a closing double qoute(\")?" << std::endl;
                    exit(1);
                }
            }

            tokens.push_back(token{.type = tokenType::dQoute, .value = buffer});
            buffer.clear();
            continue;
        }

        else if(!std::isalnum(src[index])){
            while (!std::isalnum(src[index]) && !std::isspace(src[index]) && src[index] != ';' && index < src.size()){
                buffer += take();

                if(src[index] == '(' || src[index] == ')'){
                    break;
                }
            }

            if(buffer == "+="){
                tokens.push_back(token{.type = tokenType::addEq});
            }

            else if(buffer == "-="){
                tokens.push_back(token{.type = tokenType::subEq});
            }

            else if(buffer == "*="){
                tokens.push_back(token{.type = tokenType::mulEq});
            }

            else if(buffer == "/="){
                tokens.push_back(token{.type = tokenType::divEq});
            }

            else if(buffer == "++"){
                tokens.push_back(token{.type = tokenType::pp});
            }

            else if(buffer == "--"){
                tokens.push_back(token{.type = tokenType::mm});
            }

            else if(buffer == "%="){
                tokens.push_back(token{.type = tokenType::percentEq});
            }

            else if(buffer == "="){
                tokens.push_back(token{.type = tokenType::equal});
            }

            else if(buffer == "+"){
                tokens.push_back(token{.type = tokenType::add});
            }

            else if(buffer == "-"){
                tokens.push_back(token{.type = tokenType::sub});
            }

            else if(buffer == "*"){
                tokens.push_back(token{.type = tokenType::mul});
            }

            else if(buffer == "/"){
                tokens.push_back(token{.type = tokenType::div});
            }

            else if(buffer == "%"){
                tokens.push_back(token{.type = tokenType::percent});
            }

            else if(buffer == "("){
                tokens.push_back(token{.type = tokenType::parenOpen});
            }

            else if(buffer == ")"){
                tokens.push_back(token{.type = tokenType::parenClose});
            }

            else if(buffer == "{"){
                tokens.push_back(token{.type = tokenType::curlyOpen});
            }

            else if(buffer == "}"){
                tokens.push_back(token{.type = tokenType::curlyClose});
            }

            else if(buffer == "=="){
                tokens.push_back(token{.type = tokenType::bEqual});
            }

            else if(buffer == "!="){
                tokens.push_back(token{.type = tokenType::bNotEq});
            }

            else if(buffer == "&&"){
                tokens.push_back(token{.type = tokenType::_and});
            }

            else if(buffer == "||"){
                tokens.push_back(token{.type = tokenType::_or});
            }

            else if(buffer == ">"){
                tokens.push_back(token{.type = tokenType::g});
            }

            else if(buffer == ">="){
                tokens.push_back(token{.type = tokenType::gEq});
            }

            else if(buffer == "<"){
                tokens.push_back(token{.type = tokenType::l});
            }

            else if(buffer == "<="){
                tokens.push_back(token{.type = tokenType::lEq});
            }

            else if(buffer == "!"){
                tokens.push_back(token{.type = tokenType::bNot});
            }

            else{
                std::cerr << "Error, not a valid operator '" << buffer << "'." << std::endl;
                exit(1);
            }

            buffer.clear();
            continue;
        }

        else{
            std::cerr << "Error: unknown symbol: '" << src[index] << "'." << std::endl;
            exit(1);
        }

    }

    index = 0;
    return tokens;
}

inline char lexer::take()
{
    return src.at(index++);
}
