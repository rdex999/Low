#pragma once

int genAsm::genExpr(int valsIdx)
{
    for(int i=valsIdx; i<prog->sts.at(index).vals.size(); i++)
    {
        switch (prog->sts.at(index).vals.at(i).type)
        {
        case tokenType::parenOpen:
            i = genExpr(i+1);
            break;
        
        case tokenType::parenClose:
            return i; 
        
        case tokenType::add:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use plus(+) operator without a value." << std::endl;
                exit(1);
            }

            push("rdi", 8);
            i = genSingle(i + 1, "rbx");
            pop("rdi", 8);
            outAsm << "add rdi, rbx\n\t";

            break;
        }

        case tokenType::sub:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use minus(-) operator without a value." << std::endl;
                exit(1);
            }

            push("rdi", 8);
            i = genSingle(i + 1, "rbx");
            pop("rdi", 8);
            outAsm << "sub rdi, rbx\n\t";

            break;
        }

        case tokenType::mul:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use multiplication(*) operator without a value." << std::endl;
                exit(1);
            } 

            outAsm << "mov rax, rdi\n\t";
            i = genMulDiv(i);
            outAsm << "mov rdi, rax\n\t";
            break;
        }

        case tokenType::div:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use division(/) operator without a value." << std::endl;
                exit(1);
            }

            outAsm << "mov rax, rdi\n\t";
            i = genMulDiv(i);
            outAsm << "mov rdi, rax\n\t";
            break;
        }

        case tokenType::percent:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use modulo(%) operator without a value." << std::endl;
                exit(1);
            }

            outAsm << "mov rax, rdi\n\t";
            i = genMulDiv(i);
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
            return i;


        default:
            i = genSingle(i, "rdi");
            break;
        }
    }
    return -1;
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