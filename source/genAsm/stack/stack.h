#pragma once

void genAsm::push(const char *reg, int size, const char *word)
{
    outAsm << "mov " << word << " [rsp + " << stackLoc << "], " << reg << "\n\t";
    stackLoc += size;
}

void genAsm::pop(const char *reg, int size, const char* word)
{
    stackLoc -= size;
    outAsm << "mov " << reg << ", " << word << "[rsp + " << stackLoc << "]\n\t";
}

inline void genAsm::genCurly(int idx)
{
    if(prog->sts.at(index).vals.at(idx).type == tokenType::curlyOpen){

        scopeStackLoc.push_back(stackLoc);

    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::curlyClose){

        stackLoc = scopeStackLoc.at(scopeStackLoc.size() - 1);
        scopeStackLoc.pop_back();


        for(std::map<std::string, var>::iterator itr = vars.begin(); itr != vars.end();){
            if(itr->second.scope > scopeStackLoc.size()){
                vars.erase(itr++);
            }else{
                ++itr;
            }
        }

    }
}
