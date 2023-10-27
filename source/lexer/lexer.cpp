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
    while(peek().has_value())
    {
        if(std::isalpha(peek().value())){
            while(peek().has_value() && std::isalnum(peek().value())){
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
        
        }else if(std::isdigit(peek().value())){
            while(peek().has_value() && std::isdigit(peek().value())){
                buffer += take();
            }
            tokens.push_back(token{.type = tokenType::intLit, .value = buffer});
            buffer.clear();
            continue;
        }

        else if(std::isspace(peek().value())){
            take();
            continue;
        
        }else if(peek().value() == ';'){
            take();
            tokens.push_back(token{.type = tokenType::semicolon});
            continue;
        }
        else if(!std::isalnum(peek().value())){
            while (!std::isalnum(peek().value()) && !std::isspace(peek().value())){
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
