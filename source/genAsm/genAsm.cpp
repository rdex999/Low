#include "genAsm.h"
#include "../macros/macros.h"

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

        case tokenType::ident:
            genUpdateIdent();
            break;
        
        default:
            break;
        }
    }

    outAsm << "mov rax, 60\n\tmov rdi, 0\n\tsyscall";
}

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

    return -1;
}

int genAsm::genSingle(int idx, const char* reg)
{
    int retIdx = idx;
    switch (prog->sts.at(index).vals.at(idx).type)
    {
    case tokenType::intLit:
        outAsm << "mov " << selectReg(reg, 4) << ", " << prog->sts.at(index).vals.at(idx).value << "\n\t";
        break;

    case tokenType::ident:
        outAsm << "mov " << selectReg(reg, vars[prog->sts.at(index).vals.at(idx).value].size) <<
            ", " << selectWord(vars[prog->sts.at(index).vals.at(idx).value].size) <<
            " [rsp + " << (int)(stackLoc - vars[prog->sts.at(index).vals.at(idx).value].stackLoc) << "]\n\t";
        break;

    case tokenType::parenOpen:
        retIdx = genExpr(idx + 1);
        break;

    default:
        std::cerr << "Error, unknown. (Forgot a semicolon?)" << std::endl;
        exit(1);
        break;
    }

    if(prog->sts.at(index).vals.size() > idx + 1 && 
        (prog->sts.at(index).vals.at(idx + 1).type == tokenType::mul ||
        prog->sts.at(index).vals.at(idx + 1).type == tokenType::div))
    {
        bool isRax = true;
        if(std::string("rax") != reg){
            outAsm << "mov rax, " << reg << "\n\t";
            isRax = false; 
        }
        retIdx = genMulDiv(idx + 1);
        if(!isRax){
            outAsm << "mov " << reg << ", rax\n\t";
        }
    }
    return retIdx;
}

void genAsm::push(const char *reg, int size, const char* word)
{
    outAsm << "mov " << word << " [rsp + " << stackLoc << "], " << reg << "\n\t";
    stackLoc += size;
}

void genAsm::pop(const char *reg, int size, const char* word)
{
    stackLoc -= size;
    outAsm << "mov " << reg << ", " << word << "[rsp + " << stackLoc << "]\n\t";
}

std::string genAsm::selectReg(const char *reg, int size)
{
    std::stringstream ss;
    switch (size)
    {
    case 8:
        ss << 'r' << reg[1] << reg[2];
        break;

    case 4:
        ss << 'e' << reg[1] << reg[2];
        break; 

    default:
        return "";
    }

    return ss.str();
}

std::string genAsm::selectWord(int size)
{
    switch (size)
    {
    case 8:
        return "QWORD";

    case 4:
        return "DWORD";

    case 2:
        return "WORD";

    default:
        return "";
    }
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

        vars[prog->sts.at(index).vals.at(0).value].stackLoc = stackLoc + SIZE_INT;
        vars[prog->sts.at(index).vals.at(0).value].size = 4;
        genExpr(2);
        push("edi", SIZE_INT, "DWORD");

    }else{

        // will be possible in the future tho
        std::cerr << "Error, cannot declare variable without a value." << std::endl;
        exit(1);
    }
}

inline void genAsm::genUpdateIdent()
{
    if(!vars.contains(prog->sts.at(index).key.value)){
        std::cerr << "Error, '" << prog->sts.at(index).key.value << "' is not declared." << std::endl;
        exit(1);
    }

    genExpr(1);
            // the operator, = += -= *= /= 
    switch (prog->sts.at(index).vals.at(0).type)
    {
    case tokenType::equal:
        outAsm << "mov " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " <<
            (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc) << "], " <<
            selectReg("rdi", vars[prog->sts.at(index).key.value].size) << "\n\t";
        break;

    case tokenType::addEq:
        outAsm << "add " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " <<
            (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc) << "], " <<
            selectReg("rdi", vars[prog->sts.at(index).key.value].size) << "\n\t";
        break;
    
    case tokenType::subEq:
        outAsm << "sub " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " <<
            (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc) << "], " <<
            selectReg("rdi", vars[prog->sts.at(index).key.value].size) << "\n\t";
        break;
    
    case tokenType::mulEq:
        outAsm << "mov rax, rdi\n\t";

        outAsm << "mul " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " <<
            (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc) << "]\n\t";

        outAsm << "mov " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " << (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc) << "], " << 
            selectReg("rax", vars[prog->sts.at(index).key.value].size) << "\n\t";
        break;

    case tokenType::divEq:
        outAsm << "mov rdx, 0\n\t";

        outAsm << "mov " << selectReg("rax", vars[prog->sts.at(index).key.value].size) <<
            ", " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " << (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc)
            << "]\n\t";

        outAsm << "div rdi\n\t";
        outAsm << "mov " << selectWord(vars[prog->sts.at(index).key.value].size) <<
            " [rsp + " << (int)(stackLoc - vars[prog->sts.at(index).key.value].stackLoc) << "], " <<
            selectReg("rax", vars[prog->sts.at(index).key.value].size) << "\n\t";
        break;

    default:
        break;
    }
}