#pragma once

exprRes genAsm::genExpr(size_t stmtIdx, int valsIdx)
{
    bool isPrevOp = true;
    exprRes res = exprRes{.retIdx = valsIdx}; 
    eType type = getType(stmtIdx, valsIdx);
    if(type.type == (tokenType)0){
        std::cerr << "Error, no expresion type." << std::endl;
        exit(1);
    }

    for(; res.retIdx<prog->sts.at(stmtIdx).vals.size(); ++res.retIdx)
    {
        switch (prog->sts.at(stmtIdx).vals.at(res.retIdx).type)
        {
        case tokenType::parenOpen:
            res = genExpr(stmtIdx, res.retIdx+1);
            isPrevOp = false;
            break;
        
        case tokenType::add:{
            isPrevOp = true;
            if(res.retIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use plus(+) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float && type.ptrReadBytes == -1){
                push("xmm0", 4, "", "movss");
                res.retIdx = genSingle(res.retIdx + 1, "xmm0", stmtIdx);
                pop("xmm1", 4, "", "movss");
                outAsm << "addss xmm0, xmm1\n\t";
            }else{
                push("rax", 8);
                res.retIdx = genSingle(res.retIdx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "add rax, rbx\n\t";
            }

            break;
        }

        case tokenType::sub:{
            isPrevOp = true;
            if(res.retIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use minus(-) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float && type.ptrReadBytes == -1){
                push("xmm0", 4, "", "movss");
                res.retIdx = genSingle(res.retIdx + 1, "xmm0", stmtIdx);
                pop("xmm1", 4, "", "movss");
                outAsm << "subss xmm1, xmm0\n\t";
                outAsm << "movss xmm0, xmm1\n\t";
            }else{
                push("rax", 8);
                res.retIdx = genSingle(res.retIdx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "sub rax, rbx\n\t";
            }

            break;
        }

        case tokenType::mul:{ // aka star(*) (i will change it to star in the future)
            if(res.retIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use multiplication/derefrence(*) operator without a value." << std::endl;
                exit(1);
            }

            // if the previous thing is an operator, then treat this * as a pointer
            if(isPrevOp){
                res.retIdx = genSingle(res.retIdx, "rax", stmtIdx);
                break;
            }

            if(type.type == tokenType::_float){
                res.retIdx = genMulDiv(res.retIdx, stmtIdx, tokenType::_float);
            }else{
                res.retIdx = genMulDiv(res.retIdx, stmtIdx, type.type);
            } 
            isPrevOp = true;
            break;
        }

        case tokenType::div:{
            isPrevOp = true;
            if(res.retIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use division(/) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float){
                res.retIdx = genMulDiv(res.retIdx, stmtIdx, tokenType::_float);
            }else{
                res.retIdx = genMulDiv(res.retIdx, stmtIdx, type.type);
            }
            break;
        }

        case tokenType::percent:{
            isPrevOp = true;
            if(res.retIdx + 1 >= prog->sts.at(stmtIdx).vals.size()){
                std::cerr << "Error, cannot use modulo(%) operator without a value." << std::endl;
                exit(1);
            }

            if(type.type == tokenType::_float){
                std::cerr << "Error, cannot use modulo operator (%) on floats." << std::endl;
                exit(1);
            }

            res.retIdx = genMulDiv(res.retIdx, stmtIdx, type.type);
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
            return res;


        default:
            res.retIdx = genSingle(res.retIdx, "rax", stmtIdx);
            isPrevOp = false;
            break;
        }
    }
    return res;
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
                idx = genExpr(stmtIdx, idx + 2).retIdx;
                pop("rbx", 8);
                outAsm << "mul rbx\n\t";
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
                idx = genExpr(stmtIdx, idx + 2).retIdx;
                outAsm << "mov rbx, rax\n\t";
                pop("rax", 8);
                outAsm << "xor rdx, rdx\n\tdiv rbx\n\t";
                return idx;
            }else{
                push("rax", 8);
                idx = genSingle(idx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "xor rdx, rdx\n\tdiv rbx\n\t";
                return idx;
            } 
        }
        else if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::percent){
            if(prog->sts.at(stmtIdx).vals.at(idx+1).type == tokenType::parenOpen){
                push("rax", 8);
                idx = genExpr(stmtIdx, idx + 2).retIdx;
                outAsm << "mov rbx, rax\n\t";
                pop("rax", 8);
                outAsm << "xor rdx, rdx\n\tdiv rbx\n\t";
                outAsm << "mov rax, rdx\n\t";
                return idx;
            }else{
                push("rax", 8);
                idx = genSingle(idx + 1, "rbx", stmtIdx);
                pop("rax", 8);
                outAsm << "xor rdx, rdx\n\tdiv rbx\n\t";
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
                idx = genExpr(stmtIdx, idx + 2).retIdx;
                pop("xmm1", 4, "movss");
                outAsm << "mulss xmm0, xmm1\n\t";
                return idx;
            }else{
                push("xmm0", 4, "", "movss");
                idx = genSingle(idx + 1, "xmm0", stmtIdx); // passing xmm0 has no effect
                pop("xmm1", 4, "", "movss");
                outAsm << "mulss xmm0, xmm1\n\t";
                return idx;
            }

        }else if(prog->sts.at(stmtIdx).vals.at(idx).type == tokenType::div){
            if(prog->sts.at(stmtIdx).vals.at(idx+1).type == tokenType::parenOpen){
                push("xmm0", 4, "", "movss");
                idx = genExpr(stmtIdx, idx + 2).retIdx;
                pop("xmm1", 4, "", "movss");
                outAsm << "divss xmm1, xmm0\n\tmovss xmm0, xmm1\n\t";
                return idx;
            }else{
                push("xmm0", 4, "", "movss");
                idx = genSingle(idx + 1, "xmm0", stmtIdx); // passing xmm0 has no effect
                pop("xmm1", 4, "", "movss");
                outAsm << "divss xmm1, xmm0\n\t";
                outAsm << "movss xmm0, xmm1\n\t";
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