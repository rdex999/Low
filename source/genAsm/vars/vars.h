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