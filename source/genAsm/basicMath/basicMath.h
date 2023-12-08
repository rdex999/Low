#pragma once

int genAsm::genExpr(size_t stmtIdx, int valsIdx)
{
    bool isPrevOp = true;
    eType type = getType(stmtIdx, valsIdx);
    if(type.type == (tokenType)0){
        std::cerr << "Error, no expresion type." << std::endl;
        exit(1);
    }

    for(; valsIdx<prog->sts.at(stmtIdx).vals.size(); ++valsIdx)
    {
        switch (prog->sts.at(stmtIdx).vals.at(valsIdx).type)
        {
        case tokenType::parenOpen:
            valsIdx = genExpr(stmtIdx, valsIdx+1);
            isPrevOp = false;
            break;
        
        case tokenType::add:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use plus(+) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float && type.ptrReadBytes == -1){
                push("xmm0", 4, "", "movss");
                valsIdx = genSingle(valsIdx + 1, "xmm1", stmtIdx);
                pop("xmm0", 4, "", "movss");
                outAsm << "addss xmm0, xmm1\n\t";
            }else{
                push("rdi", 8);
                valsIdx = genSingle(valsIdx + 1, "rbx", stmtIdx);
                pop("rdi", 8);
                outAsm << "add rdi, rbx\n\t";
            }

            break;
        }

        case tokenType::sub:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use minus(-) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float && type.ptrReadBytes == -1){
                push("xmm0", 4, "", "movss");
                valsIdx = genSingle(valsIdx + 1, "xmm1", stmtIdx);
                pop("xmm0", 4, "", "movss");
                outAsm << "subss xmm0, xmm1\n\t";
            }else{
                push("rdi", 8);
                valsIdx = genSingle(valsIdx + 1, "rbx", stmtIdx);
                pop("rdi", 8);
                outAsm << "sub rdi, rbx\n\t";
            }

            break;
        }

        case tokenType::mul:{ // aka star(*) (i will change it to star in the future)
            if(valsIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use multiplication/derefrence(*) operator without a value." << std::endl;
                exit(1);
            }

            // if the previous thing is an operator, then treat this * as a pointer
            if(isPrevOp){
                valsIdx = genSingle(valsIdx, type.type == tokenType::_float ? "xmm0" : "rdi", stmtIdx);
                break;
            }

            if(type.type == tokenType::_float){
                valsIdx = genMulDiv(valsIdx, stmtIdx, tokenType::_float);
            }else{
                outAsm << "mov rax, rdi\n\t";
                valsIdx = genMulDiv(valsIdx, stmtIdx, type.type);
                outAsm << "mov rdi, rax\n\t";
            } 
            isPrevOp = true;
            break;
        }

        case tokenType::div:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use division(/) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float){
                valsIdx = genMulDiv(valsIdx, stmtIdx, tokenType::_float);
            }else{
                outAsm << "mov rax, rdi\n\t";
                valsIdx = genMulDiv(valsIdx, stmtIdx, type.type);
                outAsm << "mov rdi, rax\n\t";
            }
            break;
        }

        case tokenType::percent:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use modulo(%) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float){
                std::cerr << "Error, cannot use modulo operator (%) on floats." << std::endl;
                exit(1);
            }

            outAsm << "mov rax, rdi\n\t";
            valsIdx = genMulDiv(valsIdx, stmtIdx, type.type);
            outAsm << "mov rdi, rax\n\t";
            break;
        }

        case tokenType::bEqual:
        case tokenType::bNot:
        case tokenType::bNotEq:
        case tokenType::g:
        case tokenType::gEq:
        case tokenType::l:
        case tokenType::lEq:
        case tokenType::_and:
        case tokenType::_or:
        case tokenType::parenClose:
        case tokenType::curlyOpen:
        case tokenType::bracketClose:
        case tokenType::comma:
            return valsIdx;


        default:
            if(type.type == tokenType::_float){
                valsIdx = genSingle(valsIdx, "xmm0", stmtIdx);
            }else{
                valsIdx = genSingle(valsIdx, "rdi", stmtIdx);
            }
            isPrevOp = false;
            break;
        }
    }
    return valsIdx;
}

int genAsm::genMulDiv(int idx, size_t stmtIdx, tokenType type)
{
    switch (type)
    {
    case tokenType::_int:
    case tokenType::_char:{
        if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::mul){
            if(prog->sts.at(stmtIdx).vals.at(idx + 1).type == tokenType::parenOpen){
                push("rax", 8);
                idx = genExpr(stmtIdx, idx + 2);
                pop("rax", 8);
                outAsm << "mul rdi\n\t";
                return idx;
            }else{
                push("rax", 8);
                idx = genSingle(idx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "mul rbx\n\t";
                return idx;
            }

        }else if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::div){
            if(prog->sts.at(stmtIdx).vals.at(idx+1).type == tokenType::parenOpen){
                push("rax", 8);
                idx = genExpr(stmtIdx, idx + 2);
                pop("rax", 8);
                outAsm << "mov rdx, 0\n\tdiv rdi\n\t";
                return idx;
            }else{
                push("rax", 8);
                idx = genSingle(idx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "mov rdx, 0\n\tdiv rbx\n\t";
                return idx;
            } 
        }
        else if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::percent){
            if(prog->sts.at(stmtIdx).vals.at(idx+1).type == tokenType::parenOpen){
                push("rax", 8);
                idx = genExpr(stmtIdx, idx + 2);
                pop("rax", 8);
                outAsm << "mov rdx, 0\n\tdiv rdi\n\t";
                outAsm << "mov rax, rdx\n\t";
                return idx;
            }else{
                push("rax", 8);
                idx = genSingle(idx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "mov rdx, 0\n\tdiv rbx\n\t";
                outAsm << "mov rax, rdx\n\t";
                return idx;
            }
        }
        
        break;
    }

    case tokenType::_float:{
        if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::mul){
            if(prog->sts.at(stmtIdx).vals.at(idx + 1).type == tokenType::parenOpen){
                push("xmm0", 4, "", "movss");
                idx = genExpr(stmtIdx, idx + 2);
                pop("xmm1", 4, "movss");
                outAsm << "mulss xmm0, xmm1\n\t";
                return idx;
            }else{
                push("xmm0", 4, "", "movss");
                idx = genSingle(idx + 1, "xmm1", stmtIdx);
                pop("xmm0", 4, "", "movss");
                outAsm << "mulss xmm0, xmm1\n\t";
                return idx;
            }

        }else if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::div){
            if(prog->sts.at(stmtIdx).vals.at(idx+1).type == tokenType::parenOpen){
                push("xmm0", 4, "", "movss");
                idx = genExpr(stmtIdx, idx + 2);
                pop("xmm1", 4, "", "movss");
                outAsm << "divss xmm1, xmm0\n\tmovss xmm0, xmm1\n\t";
                return idx;
            }else{
                push("xmm0", 4, "", "movss");
                idx = genSingle(idx + 1, "xmm1", stmtIdx);
                pop("xmm0", 4, "", "movss");
                outAsm << "divss xmm0, xmm1\n\t";
                return idx;
            } 
        }
        else if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::percent){
            std::cerr << "Error, cannot use modulo operator on floats." << std::endl;
            exit(1);
        }
        break;
    }
    
    default:
        break;
    }

    return -1;
}