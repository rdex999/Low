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
            if(cmpType == tokenType::_int){
                push("rdi", 8);
                from = genExpr(stmtIdx, from+1) - 1;
                pop("rbx", 8);
                outAsm << "cmp rdi, rbx\n\t";
            }else if(cmpType == tokenType::_float){
                push("xmm0", 4, "", "movss");
                from = genExpr(stmtIdx, from+1) - 1;
                pop("xmm1", 4, "", "movss");
                outAsm << "ucomiss xmm1, xmm0\n\t";
            }
            if(invert){
                outAsm << "je .L" << lable << "\n\t";
            }else{
                outAsm << "jne .L" << lable << "\n\t";
            }
            break;

        case tokenType::bNotEq:
            push("rdi", 8);
            from = genExpr(stmtIdx, from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rdi, rbx\n\t";
            if(invert){
                outAsm << "jne .L" << lable << "\n\t";
            }else{
                outAsm << "je .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::g:
            push("rdi", 8);
            from = genExpr(stmtIdx, from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\t";
            if(invert){
                outAsm << "jg .L" << lable << "\n\t";
            }else{
                outAsm << "jle .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::gEq:
            push("rdi", 8);
            from = genExpr(stmtIdx, from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\t";
            if(invert){
                outAsm << "jge .L" << lable << "\n\t";
            }else{
                outAsm << "jl .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::l:
            push("rdi", 8);
            from = genExpr(stmtIdx, from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\t";
            if(invert){
                outAsm << "jl .L" << lable << "\n\t";
            }else{
                outAsm << "jge .L" << lable << "\n\t";
            }
            break;
        
        case tokenType::lEq:
            push("rdi", 8);
            from = genExpr(stmtIdx, from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\t";
            if(invert){
                outAsm << "jle .L" << lable << "\n\t";
            }else{
                outAsm << "jg .L" << lable << "\n\t";
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
                outAsm << "test rdi, rdi\n\t";
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
