#pragma once


int genAsm::genIfExpr(int from, int lable)
{
    for(; from<prog->sts.at(index).vals.size(); ++from){
        switch (prog->sts.at(index).vals.at(from).type)
        {
        case tokenType::curlyOpen:
            genCurly(from, prog->sts.at(index).vals.at(from-1).type == tokenType::_else ? true : false);
            break;

        case tokenType::bEqual:
            push("rdi", 8);
            from = genExpr(from+1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rdi, rbx\n\tjne .L" << lable << "\n\t";
            break;

        case tokenType::bNotEq:
            push("rdi", 8);
            from = genExpr(from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rdi, rbx\n\tje .L" << lable << "\n\t";
            break;
        
        case tokenType::g:
            push("rdi", 8);
            from = genExpr(from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\tjle .L" << lable << "\n\t";
            break;
        
        case tokenType::gEq:
            push("rdi", 8);
            from = genExpr(from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\tjl .L" << lable << "\n\t";
            break;
        
        case tokenType::l:
            push("rdi", 8);
            from = genExpr(from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\tjge .L" << lable << "\n\t";
            break;
        
        case tokenType::lEq:
            push("rdi", 8);
            from = genExpr(from + 1) - 1;
            pop("rbx", 8);
            outAsm << "cmp rbx, rdi\n\tjg .L" << lable << "\n\t";
            break;

        default:
            from = genExpr(from) - 1;

            // i might change this way of checking if this is a 'self check' (if x {...})
            if(prog->sts.at(index).vals.at(from+1).type == tokenType::_and ||
                prog->sts.at(index).vals.at(from+1).type == tokenType::_or ||
                prog->sts.at(index).vals.at(from+1).type == tokenType::curlyOpen ||
                prog->sts.at(index).vals.at(from+1).type == tokenType::parenClose)
            {
                outAsm << "test rdi, rdi\n\tjz .L" << lable << "\n\t";
            }
            break;
        }
    }
    return from;
}

inline void genAsm::genIf()
{
    genIfExpr(1, lableNum);
}
