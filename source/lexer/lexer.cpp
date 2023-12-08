#include "lexer.h"

lexer::lexer(std::string src)
{
    this->src = std::move(src);
    index = 0;
}

inline bool lexer::addKeyword(const char* keyword, tokenType tok)
{
    if(buffer == keyword){
        tokens.push_back(token{.type = tok});
        buffer.clear();
        return true;
    }
    return false;
}

std::vector<token> lexer::createTokens()
{
    buffer.clear();
    tokens.clear();
    for(index = 0; index < src.size();) 
    {
        if(std::isalpha(src[index])){
            while(index < src.size() && std::isalnum(src[index])){
                buffer += take();
            }

            // keyword detection
            if(addKeyword("exit", tokenType::_exit)){ continue; }
            if(addKeyword("int", tokenType::_int)){ continue; }
            if(addKeyword("char", tokenType::_char)){ continue; }
            if(addKeyword("float", tokenType::_float)){ continue; }
            if(addKeyword("ptr", tokenType::ptr)){ continue; }
            if(addKeyword("while", tokenType::_while)){ continue; }
            if(addKeyword("for", tokenType::_for)){ continue; }
            if(addKeyword("goto", tokenType::_goto)){ continue; }
            if(addKeyword("if", tokenType::_if)){ continue; }
            if(addKeyword("is", tokenType::bEqual)){ continue; }
            if(addKeyword("else", tokenType::_else)){ continue; }
            if(addKeyword("and", tokenType::_and)){ continue; }
            if(addKeyword("or", tokenType::_or)){ continue; }
            tokens.push_back(token{.type = tokenType::ident, .value = buffer});
            buffer.clear();
            continue;

        }else if(std::isdigit(src[index])){
            tokenType tType = tokenType::intLit;
            while(index < src.size() && (std::isdigit(src[index]) || src[index] == '.')){
                if(src[index] == '.'){
                    tType = tokenType::floatLit;
                }
                buffer += take();
            }
            tokens.push_back(token{.type = tType, .value = buffer});
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
            if(src.size() > index + 2){
                if(src[index+1] == '\\'){
                    switch (src[index+2])
                    {
                    case 'n':
                        buffer = "10";
                        index += 4; 
                        break;

                    case 'r':
                        buffer = "13";
                        index += 4; 
                        break;
                    
                    case 't':
                        buffer = "9";
                        index += 4;
                        break;
                     
                    case 'b':
                        buffer = "8";
                        index += 4;
                        break;

                    case 'f':
                        buffer = "12";
                        index += 4;
                        break;

                    case '\'':
                        buffer = "39";
                        index += 4;
                        break;

                    default:
                        index += 2; 
                        buffer += take();
                        if(index < src.size() && src[index] == '\''){
                            ++index;
                        }else{
                            std::cerr << "Error, cannot use single quotes for multiple characters.\nUse double quotes(\") for multiple characters."
                                << std::endl;
                    
                            exit(1);
                        }
                        break;
                    }
                }else if(src[index+2] == '\''){
                    buffer += take();
                    buffer += take();
                    buffer += take();
                }else{
                    std::cerr << "Error, cannot use single quotes for multiple characters.\nUse double quotes(\") for multiple characters."
                        << std::endl;
                    
                    exit(1);
                }
                tokens.push_back(token{.type = tokenType::quote, .value = buffer});
                buffer.clear();
                continue;
            }else{
                std::cerr << "Error, forgot a closing quote?" << std::endl;
                exit(1);
            }
        }

        else if(src[index] == '"'){
            while(index < src.size()){
                if(src[index] == '\\'){
                    buffer += take();
                }
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
                if(opOpenCloseCheck(index) == true || opOpenCloseCheck(index-1) == true){
                    break;
                }
            }
            
            if(buffer == "//"){
                buffer.clear();
                while(index < src.size()){
                    if(src[index] == '\n'){
                        break;
                    }
                    take();
                }
                continue;
            }

            if(addKeyword("+=", tokenType::addEq)){ continue; }
            if(addKeyword("-=", tokenType::subEq)){ continue; }
            if(addKeyword("*=", tokenType::mulEq)){ continue; }
            if(addKeyword("/=", tokenType::divEq)){ continue; }
            if(addKeyword("++", tokenType::pp)){ continue; }
            if(addKeyword("--", tokenType::mm)){ continue; }
            if(addKeyword("%=", tokenType::percentEq)){ continue; }
            if(addKeyword("=", tokenType::equal)){ continue; }
            if(addKeyword("+", tokenType::add)){ continue; }
            if(addKeyword("-", tokenType::sub)){ continue; }
            if(addKeyword("*", tokenType::mul)){ continue; }
            if(addKeyword("/", tokenType::div)){ continue; }
            if(addKeyword("%", tokenType::percent)){ continue; }
            if(addKeyword("&", tokenType::singleAnd)){ continue; }
            if(addKeyword("(", tokenType::parenOpen)){ continue; }
            if(addKeyword(")", tokenType::parenClose)){ continue; }
            if(addKeyword("{", tokenType::curlyOpen)){ continue; }
            if(addKeyword("}", tokenType::curlyClose)){ continue; }
            if(addKeyword("==", tokenType::bEqual)){ continue; }
            if(addKeyword("!=", tokenType::bNotEq)){ continue; }
            if(addKeyword("&&", tokenType::_and)){ continue; }
            if(addKeyword("||", tokenType::_or)){ continue; }
            if(addKeyword(">", tokenType::g)){ continue; }
            if(addKeyword(">=", tokenType::gEq)){ continue; }
            if(addKeyword("<", tokenType::l)){ continue; }
            if(addKeyword("<=", tokenType::lEq)){ continue; }
            if(addKeyword("!", tokenType::bNot)){ continue; }
            if(addKeyword("[", tokenType::bracketOpen)){ continue; }
            if(addKeyword("]", tokenType::bracketClose)){ continue; }
            if(addKeyword(",", tokenType::comma)){ continue; }
            if(addKeyword(":", tokenType::colon)){ continue; }

            std::cerr << "Error, not a valid operator '" << buffer << "'." << std::endl;
            exit(1);
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

inline bool lexer::opOpenCloseCheck(size_t idx)
{
    switch (src[idx])
    {
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
        return true; 
    
    default:
        return false; 
    }
}