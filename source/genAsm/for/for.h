#pragma once

inline void genAsm::genFor(int idx)
{
    if(index+2 >= prog->sts.size()){
        std::cerr << "Error, incomplete 'for' statement." << std::endl;
        exit(1);
    }

    size_t indexFor = index;
    size_t indexForEnd;
    size_t forLoopLable = lableNum;

    // scope starts before for statement
    scopeStackLoc.push_back(stackLoc);

    // generate first for statement ( for st1; st2; st3{...} )
    ++index;
    genStmt();

    // index+3 == statement inside for loop
    index += 3;
    size_t lableForCmp = findLableCountCurly(index)+lableNum;

    outAsm << "jmp .L" << lableForCmp << "\n\t";
    outAsm << "\r.L" << lableNum++ << ":\n\t";

    size_t curlyCount = 0;
    for(; index < prog->sts.size(); ++index){
        for(int i=0; i<prog->sts.at(index).vals.size(); ++i){
            if(prog->sts.at(index).vals.at(i).type == tokenType::curlyOpen){
                ++curlyCount;
            }else if(prog->sts.at(index).vals.at(i).type == tokenType::curlyClose){
                if(curlyCount == 0){
                    indexForEnd = index;
                    goto afterBothForGFL;
                }
                --curlyCount;
            }else if(prog->sts.at(index).vals.at(i).type == tokenType::_while){ // because the while loop generation icreases the index
                break; 
            }
        }
        genStmt();
    }

    afterBothForGFL:

    // indexFor+3 -> third statement of for
    index = indexFor+3;
    genStmt();
    
    --index; // second statement of for
    outAsm << "\r.L" << lableNum++ << ":\n\t";
    
    genIfExpr(0, forLoopLable, index, true, false);
    
    index = indexForEnd;
    genStmt();
}