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

inline void genAsm::genCurly(int idx, bool isFromElse)
{
    if(prog->sts.at(index).vals.at(idx).type == tokenType::curlyOpen){

        scopeStackLoc.push_back(stackLoc);
        lables.push_back(lableNum++);
        //if(isFromElse){
        //    outAsm << "jmp .L" << lables.at(lables.size() - 1) + 1 << "\n\t";
        //}
        //outAsm << "\r.L" << lables.at(lables.size() - 1) << ":\n\t";


    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::curlyClose){

        stackLoc = scopeStackLoc.at(scopeStackLoc.size() - 1);
        scopeStackLoc.pop_back();

        if(index + 1 < prog->sts.size() && prog->sts.at(index+1).vals.at(0).type == tokenType::_else){
            outAsm << "jmp .L" << lables.at(lables.size() - 1) + 1 << "\n\t";
        }
        outAsm << "\r.L" << lables.at(lables.size() - 1) << ":\n\t";
        lables.pop_back();

        for(std::map<std::string, var>::iterator itr = vars.begin(); itr != vars.end();){
            if(itr->second.scope > scopeStackLoc.size()){
                vars.erase(itr++);
            }else{
                ++itr;
            }
        }

    }
}
