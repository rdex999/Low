#pragma once

void* genAsm::varInScope(const std::string *varName, int scope)
{
    for(auto itr = vars.find(*varName); itr != vars.end(); ++itr){
        if(itr->first != *varName){
            break;
        }

        if(itr->second.scope == scope){
            return &(itr->second);
        }
    }
    return nullptr;
}

void *genAsm::varAccessible(const std::string *varName, int scope)
{
    var* v = nullptr;
    for(auto itr = vars.find(*varName); itr != vars.end(); ++itr){
        if(itr->first != *varName){
            break;
        }

        if(!v || (v && v->scope < itr->second.scope)){
            v = &(itr->second);
        }
    }

    if(!v){
        std::cerr << "Error, identifier '" << *varName << "' is undeclared or not declared in this scope." << std::endl;
        exit(1);
    }
    return v;
}

size_t genAsm::findLableCountCurly(size_t stmtIdx)
{
    size_t lableCount = 0;
    size_t curlyCount = 1;
    for(; stmtIdx < prog->sts.size(); ++stmtIdx){
        for(int i=0; i<prog->sts.at(stmtIdx).vals.size(); ++i){
            switch (prog->sts.at(stmtIdx).vals.at(i).type)
            {
            case tokenType::curlyOpen:
                ++curlyCount; 
                break;

            case tokenType::curlyClose:
                --curlyCount;
                ++lableCount;
                if(!curlyCount){
                    return lableCount;
                }
                break;
            
            case tokenType::_while: 
            case tokenType::_for: 
                lableCount += 2;
                break;

            default:
                break;
            }
        }
    }
    return lableCount;
}