#pragma once

int genAsm::genExpr(int valsIdx)
{
    bool isPrevOp = true;
    for(valsIdx; valsIdx<prog->sts.at(index).vals.size(); ++valsIdx)
    {
        switch (prog->sts.at(index).vals.at(valsIdx).type)
        {
        case tokenType::parenOpen:
            valsIdx = genExpr(valsIdx+1);
            isPrevOp = true;
            break;
        
        case tokenType::parenClose:
            return valsIdx; 
        
        case tokenType::add:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use plus(+) operator without a value." << std::endl;
                exit(1);
            }

            push("rdi", 8);
            valsIdx = genSingle(valsIdx + 1, "rbx");
            pop("rdi", 8);
            outAsm << "add rdi, rbx\n\t";

            break;
        }

        case tokenType::sub:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use minus(-) operator without a value." << std::endl;
                exit(1);
            }

            push("rdi", 8);
            valsIdx = genSingle(valsIdx + 1, "rbx");
            pop("rdi", 8);
            outAsm << "sub rdi, rbx\n\t";

            break;
        }

        case tokenType::mul:{ // aka star(*) (i will change it to star in the future)
            if(valsIdx + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use multiplication/derefrence(*) operator without a value." << std::endl;
                exit(1);
            }

            // if the previous thing is an operator, then treat this * as a pointer
            if(isPrevOp){
                valsIdx = genSingle(valsIdx+1, "rdi");
                outAsm << "mov rdi, [rdi]\n\t";
                isPrevOp = false;
                break;
            }

            outAsm << "mov rax, rdi\n\t";
            valsIdx = genMulDiv(valsIdx);
            outAsm << "mov rdi, rax\n\t";
            isPrevOp = true;
            break;
        }

        case tokenType::div:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use division(/) operator without a value." << std::endl;
                exit(1);
            }

            outAsm << "mov rax, rdi\n\t";
            valsIdx = genMulDiv(valsIdx);
            outAsm << "mov rdi, rax\n\t";
            break;
        }

        case tokenType::percent:{
            isPrevOp = true;
            if(valsIdx + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use modulo(%) operator without a value." << std::endl;
                exit(1);
            }

            outAsm << "mov rax, rdi\n\t";
            valsIdx = genMulDiv(valsIdx);
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
        case tokenType::curlyOpen:
            return valsIdx;


        default:
            valsIdx = genSingle(valsIdx, "rdi");
            isPrevOp = false;
            break;
        }
    }
    return valsIdx;
}

int genAsm::genMulDiv(int idx)
{
    if(prog->sts.at(index).vals.at(idx).type == tokenType::mul){
        if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::parenOpen){
            push("rax", 8);
            idx = genExpr(idx + 2);
            pop("rax", 8);
            outAsm << "mul rdi\n\t";
            return idx;
        }else{
            push("rax", 8);
            idx = genSingle(idx + 1, "rbx");
            pop("rax", 8);
            outAsm << "mul rbx\n\t";
            return idx;
        }

    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::div){
        if(prog->sts.at(index).vals.at(idx+1).type == tokenType::parenOpen){
            push("rax", 8);
            idx = genExpr(idx + 2);
            pop("rax", 8);
            outAsm << "mov rdx, 0\n\tdiv rdi\n\t";
            return idx;
        }else{
            push("rax", 8);
            idx = genSingle(idx + 1, "rbx");
            pop("rax", 8);
            outAsm << "mov rdx, 0\n\tdiv rbx\n\t";
            return idx;
        } 
    }
    else if(prog->sts.at(index).vals.at(idx).type == tokenType::percent){
        if(prog->sts.at(index).vals.at(idx+1).type == tokenType::parenOpen){
            push("rax", 8);
            idx = genExpr(idx + 2);
            pop("rax", 8);
            outAsm << "mov rdx, 0\n\tdiv rdi\n\t";
            outAsm << "mov rax, rdx\n\t";
            return idx;
        }else{
            push("rax", 8);
            idx = genSingle(idx + 1, "rbx");
            pop("rax", 8);
            outAsm << "mov rdx, 0\n\tdiv rbx\n\t";
            outAsm << "mov rax, rdx\n\t";
            return idx;
        }
    }

    return -1;
}