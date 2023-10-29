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
        else if(!std::isalnum(src[index])){
            while (!std::isalnum(src[index]) && !std::isspace(src[index])){
                buffer += take();
            }

            if(buffer == "="){
                tokens.push_back(token{.type = tokenType::equal});
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
