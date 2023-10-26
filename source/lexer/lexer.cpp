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
    while(peek().has_value())
    {
        if(std::isalpha(peek().value())){
            while(peek().has_value() && std::isalpha(peek().value())){
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
        
        }else if(std::isdigit(peek().value())){
            while(peek().has_value() && std::isdigit(peek().value())){
                buffer += take();
            }

            tokens.push_back(token{.type = tokenType::intLit, .value = buffer});
            buffer.clear();
            continue;
        
        }else if(std::isspace(peek().value())){
            take();
            continue;
        
        }else if(peek().value() == ';'){
            take();
            tokens.push_back(token{.type = tokenType::semicolon});
            continue;
        
        }else{
            std::cerr << "Error: unknown symbol: '" << peek().value() << "'." << std::endl;
            exit(1);
        }

    }

    index = 0;
    return tokens;
}

inline std::optional<char> lexer::peek(int amount)
{
    if(index + amount >= src.length()){
        return {};
    }else{
        return src.at(index + amount);
    }
}

inline char lexer::take()
{
    return src.at(index++);
}
