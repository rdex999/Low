#pragma once

void genAsm::push(const char *reg, int size, const char *word, const char* mov)
{
    stackLoc += size;
    outAsm << mov << " " << word << " [rbp - " << stackLoc << "], " << reg << "\n\t";
}

void genAsm::pop(const char *reg, int size, const char* word, const char* mov)
{
    outAsm << mov << " " << reg << ", " << word << "[rbp - " << stackLoc << "]\n\t";
    stackLoc -= size;
}

inline void genAsm::genCurly(int idx, bool isFromElse)
{
    if(prog->sts.at(index).vals.at(idx).type == tokenType::curlyOpen){

        scopeStackLoc.push_back(stackLoc);

    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::curlyClose){

        stackLoc = scopeStackLoc.at(scopeStackLoc.size() - 1);
        scopeStackLoc.pop_back();

        if(index + 1 < prog->sts.size() && 
            (prog->sts.at(index+1).vals.at(0).type == tokenType::_else ||
            prog->sts.at(index+1).vals.at(0).type == tokenType::elseIf))
        {
            size_t curlyCount = 0, lableCount = 0;
            for(size_t i = index+1; i<prog->sts.size(); ++i){
                for(int j=0; j<prog->sts.at(i).vals.size(); ++j){
                    if(prog->sts.at(i).vals.at(j).type == tokenType::curlyOpen){
                        ++curlyCount;
                    }else if(prog->sts.at(i).vals.at(j).type == tokenType::curlyClose){
                        --curlyCount;
                        ++lableCount;
                        if(curlyCount == 0){
                            if(i+1 < prog->sts.size() && 
                                (prog->sts.at(i+1).vals.at(0).type == tokenType::_else ||
                                prog->sts.at(i+1).vals.at(0).type == tokenType::elseIf))
                            {
                                break;
                            }
                            goto jmpL;
                        }
                    }else if(prog->sts.at(i).vals.at(j).type == tokenType::_while){
                        lableCount += 2;
                    }
                }
            }
            jmpL:
                outAsm << "jmp .L" << lableNum+lableCount << "\n\t";
        }
        outAsm << "\r.L" << lableNum << ":\n\t";
        ++lableNum;

        for(std::map<std::string, var>::iterator itr = vars.begin(); itr != vars.end();){
            if(itr->second.scope > scopeStackLoc.size()){
                vars.erase(itr++);
            }else{
                ++itr;
            }
        }

    }
}
