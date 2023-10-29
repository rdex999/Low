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

void genAsm::genExpr(int valsIdx, int offset)
{
    int to = offset == -1 ? prog->sts.at(index).vals.size() : (valsIdx + offset);
    for(size_t i=valsIdx; i < to; i++)
    {
        switch (prog->sts.at(index).vals.at(i).type)
        {
        case tokenType::intLit:
            outAsm << "mov rax, " << prog->sts.at(index).vals.at(i).value << "\n\t";
            push("rax");
            break;

        case tokenType::ident:
            if(vars.contains(prog->sts.at(index).vals.at(i).value)){
                outAsm << "push QWORD [rsp + " << (int)((stackLoc - vars[prog->sts.at(index).vals.at(i).value].stackLoc) * 8) << "]\n\t";
                stackLoc++;
            }else{
                std::cerr << "Error, '" << prog->sts.at(index).vals.at(i).value << "' is not defined." << std::endl;
                exit(1);
            }
            break;

        case tokenType::add:
            if(i >= prog->sts.at(index).vals.size()){
                std::cerr << "Error, cannot use plus(+) operator without a value." << std::endl;
                exit(1);
            }

            i++;
            genExpr(i, 1);
            pop("rax");
            pop("rbx");
            outAsm << "add rax, rbx\n\t";
            push("rax");
            
            break;

        default:
            // TODO: think of an error
            std::cerr << "Error, unknown." << std::endl;
            exit(1);
            break;
        }
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
        outAsm << "mov rax, 60\n\t" << "pop rdi\n\tsyscall\n\t";
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

    }else{

        // will be possible in the future tho
        std::cerr << "Error, cannot declare variable without a value." << std::endl;
        exit(1);
    }
}