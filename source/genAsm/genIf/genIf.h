#pragma once

int genAsm::genIfExpr(int from, int lable, size_t stmtIdx, bool invert, bool handleCurly)
{
    tokenType cmpType = tokenType::_int;
    for(; from<prog->sts.at(stmtIdx).vals.size(); ++from){
        switch (prog->sts.at(stmtIdx).vals.at(from).type)
        {
        case tokenType::curlyOpen:
            if(handleCurly){
                genCurly(from, false);
            }
            break;

        case tokenType::bEqual:
            from = cmpTwo(stmtIdx, from, cmpType);
            if(invert){
                outAsm << "je .L" << lable << "\n\t";
            }else{
                outAsm << "jne .L" << lable << "\n\t";
            }
            break;

        case tokenType::bNotEq:
            from = cmpTwo(stmtIdx, from, cmpType);
            if(invert){
                outAsm << "jne .L" << lable << "\n\t";
            }else{
                outAsm << "je .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::g:
            from = cmpTwo(stmtIdx, from, cmpType);
            if(invert){
                outAsm << selectJump(tokenType::g, cmpType) << " .L" << lable << "\n\t";
            }else{
                outAsm << selectJump(tokenType::lEq, cmpType) << " .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::gEq:
            from = cmpTwo(stmtIdx, from, cmpType);
            if(invert){
                outAsm << selectJump(tokenType::gEq, cmpType) << " .L" << lable << "\n\t";
            }else{
                outAsm << selectJump(tokenType::l, cmpType) << " .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::l:
            from = cmpTwo(stmtIdx, from, cmpType);
            if(invert){
                outAsm << selectJump(tokenType::l, cmpType) << " .L" << lable << "\n\t";
            }else{
                outAsm << selectJump(tokenType::gEq, cmpType) << " .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::lEq:
            from = cmpTwo(stmtIdx, from, cmpType);
            if(invert){
                outAsm << selectJump(tokenType::lEq, cmpType) << " .L" << lable << "\n\t";
            }else{
                outAsm << selectJump(tokenType::g, cmpType) << " .L" << lable << "\n\t";
            }
            break;

        default:
            for(int i=from; i<prog->sts.at(stmtIdx).vals.size(); ++i){
                switch (prog->sts.at(stmtIdx).vals.at(i).type)
                {
                case tokenType::_int:
                case tokenType::intLit:
                    cmpType = tokenType::_int; 
                    goto afterCmpCheck;
                    break;

                case tokenType::_float:
                case tokenType::floatLit:
                    cmpType = tokenType::_float; 
                    goto afterCmpCheck;
                    break;

                default:
                    if(prog->sts.at(stmtIdx).vals.at(i).type >= tokenType::_or &&
                        prog->sts.at(stmtIdx).vals.at(i).type <= tokenType::lEq)
                    {
                        std::cerr << "Error, unknows compare size. Try casting to somethig." << std::endl;
                        exit(1);
                    }
                    break;
                }
            }
            afterCmpCheck:
            from = genExpr(stmtIdx, from) - 1;

            // i might change this way of checking if this is a 'self check' (if x {...})
            if(prog->sts.at(stmtIdx).vals.at(from+1).type == tokenType::_and ||
                prog->sts.at(stmtIdx).vals.at(from+1).type == tokenType::_or ||
                prog->sts.at(stmtIdx).vals.at(from+1).type == tokenType::curlyOpen ||
                prog->sts.at(stmtIdx).vals.at(from+1).type == tokenType::parenClose)
            {
                if(cmpType == tokenType::_int){
                    outAsm << "test rdi, rdi\n\t";
                }else if(cmpType == tokenType::_float){
                    outAsm << "ptest xmm0, xmm0\n\t";
                }
                if(invert){
                    outAsm << "jnz .L" << lable << "\n\t";
                }else{
                    outAsm << "jz .L" << lable << "\n\t";
                }
            }
            break;
        }
    }
    return from;
}

int genAsm::cmpTwo(size_t stmtIdx, int idx, tokenType cmpType)
{
    switch (cmpType)
    {
    case tokenType::_int:
        push("rdi", 8);
        idx = genExpr(stmtIdx, idx+1) - 1;
        pop("rbx", 8);
        outAsm << "cmp rbx, rdi\n\t";
        break;

    case tokenType::_float:
        push("xmm0", 4, "", "movss");
        idx = genExpr(stmtIdx, idx+1) - 1;
        pop("xmm1", 4, "", "movss");
        outAsm << "ucomiss xmm1, xmm0\n\t";
        break;

    default:
        break;
    }
    return idx;
}

std::string genAsm::selectJump(tokenType cond, tokenType cmpType)
{
    switch (cond)
    {
        case tokenType::g:
            switch (cmpType){
            case tokenType::_int:
                return "jg";

            case tokenType::_float:
                return "ja";

            default:
                break;
            } 
            break;

        case tokenType::gEq:
            switch(cmpType){
            case tokenType::_int:
                return "jge";

            case tokenType::_float:
                return "jae";

            default:
                break;
            }

        case tokenType::l:
            switch(cmpType){
            case tokenType::_int:
                return "jl";

            case tokenType::_float:
                return "jb";
            }

        case tokenType::lEq:
            switch (cmpType){
            case tokenType::_int:
                return "jle";

            case tokenType::_float:
                return "jbe";

            default:
                break;
            }

        default:
            break;
    }
    return "";
}

inline void genAsm::genIf()
{
    size_t lableCount = 0, curlyCount = 0;
    for(size_t i = index; i<prog->sts.size() ; ++i){
        for(int j=0; j<prog->sts.at(i).vals.size(); ++j){
            if(prog->sts.at(i).vals.at(j).type == tokenType::curlyOpen){
                ++curlyCount;
            }else if(prog->sts.at(i).vals.at(j).type == tokenType::curlyClose){
                --curlyCount; 
                ++lableCount;
                if(curlyCount == 0){
                    goto genIfL;
                }
            }else if(prog->sts.at(i).vals.at(j).type == tokenType::_while){
                lableCount += 2;
            }
        }
    }
    genIfL:
        genIfExpr(1, lableNum+lableCount-1, index);
}
