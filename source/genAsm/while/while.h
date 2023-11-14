#pragma once
#include "../genAsm.h"

inline void genAsm::genWhile(int idx)
{
    size_t whileIdx = index;
    size_t lableIdx = lableNum;
    outAsm << "\r.L" << lableNum++ << ":\n\t";
    
    size_t lableCount = 0, curlyCount = 0;
    for(; index<prog->sts.size(); ++index){
        for(int i=idx; i<prog->sts.at(index).vals.size(); ++i){
            if(prog->sts.at(index).vals.at(i).type == tokenType::curlyOpen){
                ++curlyCount;
                if(whileIdx == index){
                    genCurly(i);
                }
            }else if(prog->sts.at(index).vals.at(i).type == tokenType::_while){
                lableCount += 2;
            }else if(prog->sts.at(index).vals.at(i).type == tokenType::curlyClose){
                --curlyCount;
                ++lableCount;
                if(!curlyCount){
                    outAsm << "\r.L" << lableNum++ << ":\n\t";
                    genIfExpr(1, lableIdx, whileIdx, true, false);
                    genCurly(i);
                    return;                    
                }
            }
        }
        if(index != whileIdx){
            genStmt();
        }
    }
}