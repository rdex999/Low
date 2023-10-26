#include "genAsm.h"

genAsm::genAsm(const node::program* prog)
{
    this->prog = prog;

    outAsm += "bits 64\n\nsection .text\n\tglobal _start\n\n_start:\n\t";

    for(index = 0; index < prog->sts.size(); index++)
    {
        switch (prog->sts.at(index).key.type)
        {
        case tokenType::_exit:
            genExit();
            break;
        
        default:
            break;
        }
    }
}

inline void genAsm::genExit()
{
    std::stringstream stream;
    stream << "mov rax, 60\n\tmov rdi, " << prog->sts.at(index).vals.at(0).value << "\n\tsyscall";
    outAsm += stream.str();
}

inline void genAsm::pusha()
{
}

inline void genAsm::popa()
{
}
