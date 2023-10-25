#include "lexer.h"

lexer::lexer(const std::string* src)
{
    this->src = *src;
    index = 0;
}

std::vector<token> lexer::createTokens()
{
    std::string buffer;
    std::vector<token> tokens;
    while(peak().has_value())
    {
        if(std::isalpha(peak().value())){
            while(peak().has_value() && std::isalpha(peak().value())){
                buffer += take();
            }

            // keyword detection
            if(buffer == "exit"){
                tokens.push_back(token{.type = tokenType::_exit});
            }else{
                std::cerr << "Error: no such keyword \"" << buffer << "\"." << std::endl;
                exit(1);
            }
            buffer.clear();
            continue;
        
        }else if(std::isdigit(peak().value())){
            while(peak().has_value() && std::isdigit(peak().value())){
                buffer += take();
            }

            tokens.push_back(token{.type = tokenType::intLit, .value = buffer});
            buffer.clear();
            continue;
        
        }else if(std::isspace(peak().value())){
            take();
            continue;
        
        }else if(peak().value() == ';'){
            take();
            tokens.push_back(token{.type = tokenType::semicolon});
            continue;
        
        }else{
            std::cerr << "Error: unknown symbol: '" << peak().value() << "'." << std::endl;
            exit(1);
        }

    }

    index = 0;
    return tokens;
}

inline std::optional<char> lexer::peak(int amount)
{
    if(index + amount > src.length()){
        return {};
    }else{
        return src.at(index);
    }
}

inline char lexer::take()
{
    return src.at(index++);
}
