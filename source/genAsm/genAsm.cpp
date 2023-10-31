#include "genAsm.h"

genAsm::genAsm(const node::program* prog)
{
    this->prog = prog;

    outAsm << "bits 64\n\nsection .text\n\tglobal _start\n\n_start:\n\t";

    for(index = 0; index < prog->sts.size(); index++)
    {
        switch (prog->sts.at(index).key.type)
        {
        case tokenType::_exit:
            genExit();
            break;

        case tokenType::_int:
            genInt();
            break;
        
        default:
            break;
        }
    }

    outAsm << "mov rax, 60\n\tmov rdi, 0\n\tsyscall";
}

void genAsm::genExpr(int valsIdx)
{
    for(int i=valsIdx; i<prog->sts.at(index).vals.size(); i++)
    {
        switch (prog->sts.at(index).vals.at(i).type)
        {
        case tokenType::add:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use plus(+) operator without a value." << std::endl;
                exit(1);
            }

            // i+2 is the next operator
            if(i+2 < prog->sts.at(index).vals.size() &&
                (prog->sts.at(index).vals.at(i+2).type == tokenType::mul ||
                prog->sts.at(index).vals.at(i+2).type == tokenType::div))
            {
                push("rdi");
                if(!genSingle(i+1, "rax")){
                    pop("rax");
                }
                genMulDiv(i+2);
                pop("rdi");
                outAsm << "add rdi, rax\n\t";
                i += 3;
                break;
            }else{
                if(!genSingle(i+1, "rbx")){
                    pop("rbx");
                }
                outAsm << "add rdi, rbx\n\t";
                i++;
            }
            break;
        }

        case tokenType::sub:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use minus(-) operator without a value." << std::endl;
                exit(1);
            }

            // i+2 is the next operator
            if(i+2 < prog->sts.at(index).vals.size() &&
                (prog->sts.at(index).vals.at(i+2).type == tokenType::mul ||
                prog->sts.at(index).vals.at(i+2).type == tokenType::div))
            {
                push("rdi");
                if(!genSingle(i+1, "rax")){
                    pop("rax");
                }
                genMulDiv(i+2);
                pop("rdi");
                outAsm << "sub rdi, rax\n\t";
                i += 3;
                break;
            }else{
                if(!genSingle(i+1, "rbx")){
                    pop("rbx");
                }
                outAsm << "sub rdi, rbx\n\t";
                i++;
            }
            break;
        }

        case tokenType::mul:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use multiplication(*) operator without a value." << std::endl;
                exit(1);
            } 

            outAsm << "mov rax, rdi\n\t";
            genMulDiv(i);
            outAsm << "mov rdi, rax\n\t";
            i++;
            break;
        }

        case tokenType::div:{
            if(i + 1 >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use division(/) operator without a value." << std::endl;
                exit(1);
            } 

            outAsm << "mov rax, rdi\n\t";
            genMulDiv(i);
            outAsm << "mov rdi, rax\n\t";
            i++;
            break;
        }

        default:
            if(!genSingle(i, "rdi")){
                pop("rdi");
            }
            break;
        }
    }
}

void genAsm::genMulDiv(int idx)
{
    if(prog->sts.at(index).vals.at(idx).type == tokenType::mul){
        if(!genSingle(idx+1, "rbx")){
            pop("rbx");
        }

        outAsm << "mul rbx\n\t";
        
    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::div){
        if(!genSingle(idx+1, "rbx")){
            pop("rbx");
        }
        outAsm << "mov rdx, 0\n\t";
        outAsm << "div rbx\n\t";
    }
}

bool genAsm::genSingle(int idx, const char* reg)
{
    switch (prog->sts.at(index).vals.at(idx).type)
    {
    case tokenType::intLit:
        outAsm << "mov " << reg << ", " << prog->sts.at(index).vals.at(idx).value << "\n\t";
        return true;
        break;

    case tokenType::ident:
        outAsm << "push QWORD [rsp + " << (int)((stackLoc - vars[prog->sts.at(index).vals.at(idx).value].stackLoc) * 8) << "]\n\t";
        stackLoc++; 
        return false; 
        break;

    default:
        std::cerr << "Error, unknown. (Forgot a semicolon?)" << std::endl;
        exit(1);
        break;
    }
}

void genAsm::push(const char *reg)
{
    outAsm << "push " << reg << "\n\t";
    stackLoc++;
}

void genAsm::pop(const char *reg)
{
    outAsm << "pop " << reg << "\n\t";
    stackLoc--;
}

inline void genAsm::genExit()
{
    if(prog->sts.at(index).vals.at(0).type == tokenType::intLit && prog->sts.at(index).vals.size() == 1){
        outAsm << "mov rax, 60\n\tmov rdi, " << prog->sts.at(index).vals.at(0).value << "\n\tsyscall\n\t";
    }else{
        genExpr();
        outAsm << "mov rax, 60\n\t";
        outAsm << "syscall\n\t";
    }
}

inline void genAsm::genInt()
{
    if(prog->sts.at(index).vals.size() > 1 && vars.contains(prog->sts.at(index).vals.at(0).value)){
        std::cerr << "Error, variable '" << prog->sts.at(index).vals.at(0).value << "' has already been declared." << std::endl;
        exit(1);
    }

    if(2 < prog->sts.at(index).vals.size() &&
        prog->sts.at(index).vals.at(0).type == tokenType::ident &&
        prog->sts.at(index).vals.at(1).type == tokenType::equal)
    {

        vars[prog->sts.at(index).vals.at(0).value].stackLoc = stackLoc+1;
        genExpr(2);
        push("rdi");

    }else{

        // will be possible in the future tho
        std::cerr << "Error, cannot declare variable without a value." << std::endl;
        exit(1);
    }
}