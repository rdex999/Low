#pragma once
#include "../genAsm.h"

inline void genAsm::genWhile(int idx)
{
    size_t orgLable = lableNum, orgStmtIdx = index;

    size_t lableCount = 0, curlyCount = 0;
    size_t indexCopy = index;
    for(; indexCopy < prog->sts.size(); ++indexCopy){
        for(int i=0; i<prog->sts.at(indexCopy).vals.size(); ++i){
            if(prog->sts.at(indexCopy).vals.at(i).type == tokenType::curlyOpen){
                ++curlyCount;
                if(indexCopy == index){
                    genCurly(i);
                }
            }
            if(prog->sts.at(indexCopy).vals.at(i).type == tokenType::_while){
                lableCount += 2;
            }
            if(prog->sts.at(indexCopy).vals.at(i).type == tokenType::curlyClose){
                --curlyCount;
                ++lableCount;
                if(!curlyCount){
                    goto afterLableCounted;
                }
            }
        }
    }
    afterLableCounted:
        outAsm << "jmp .L"  << lableNum + lableCount - 2 << "\n\t";
        outAsm << "\r.L" << lableNum++ << ":\n\t";
        for(++index; index<prog->sts.size(); ++index){
            if(index >= indexCopy){
                outAsm << "\r.L" << lableNum++ << ":\n\t";
                genIfExpr(1, orgLable, orgStmtIdx, true, false);
                genStmt();
                break;
            }
            genStmt();
        }

}