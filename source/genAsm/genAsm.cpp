#include "genAsm.h"
#include "../macros/macros.h"
#include "basicMath/basicMath.h"
#include "vars/vars.h"
#include "stack/stack.h"
#include "registers/registers.h"
#include "genIf/genIf.h"
#include "int/int.h"
#include "ident/ident.h"
#include "char/char.h"

genAsm::genAsm(const node::program* prog)
{
    this->prog = prog;

    outAsm << "bits 64\n\nsection .text\n\tglobal _start\n\n_start:\n\t";

    for(index = 0; index < prog->sts.size(); index++)
    {
        genStmt();
    }

    outAsm << "mov rax, 60\n\tmov rdi, 0\n\tsyscall";
}

void genAsm::genStmt()
{
    switch (prog->sts.at(index).vals.at(0).type)
    {
    case tokenType::_exit:
        genExit();
        break;

    case tokenType::_int:
        genInt();
        break;

    case tokenType::_char:
        genChar();
        break;

    case tokenType::ident:
        genUpdateIdent();
        break;

    case tokenType::curlyOpen:
    case tokenType::curlyClose:
        genCurly();
        break;

    case tokenType::pp:
    case tokenType::mm:
        genPreIncDec(0);
        break;

    case tokenType::_if:
        genIf();
        break;

    default:
        break;
    }
}

int genAsm::genSingle(int idx, const char* reg)
{
    int retIdx = idx;

    if(prog->sts.at(index).vals.size() > retIdx + 1 &&
        (prog->sts.at(index).vals.at(retIdx + 1).type == tokenType::pp ||
        prog->sts.at(index).vals.at(retIdx + 1).type == tokenType::mm))
    {
        genPostIncDec(retIdx++, reg);
        goto checkMulDiv;

    }else if(prog->sts.at(index).vals.at(retIdx).type == tokenType::pp ||
        prog->sts.at(index).vals.at(retIdx).type == tokenType::mm)
    {
        genPreIncDec(retIdx++, reg);
        goto checkMulDiv;
    }

    switch (prog->sts.at(index).vals.at(retIdx).type)
    {
    case tokenType::intLit:
        outAsm << "mov " << selectReg(reg, 4) << ", " << prog->sts.at(index).vals.at(retIdx).value << "\n\t";
        break;

    case tokenType::quote:
        outAsm << "mov " << selectReg(reg, 1) << ", " << prog->sts.at(index).vals.at(retIdx).value << "\n\t";
        break;

    case tokenType::ident:{

        var* v = (var*)varAccessible(&(prog->sts.at(index).vals.at(retIdx).value), scopeStackLoc.size());

        outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "]\n\t";

        break;
    }

    case tokenType::singleAnd:{
        if(retIdx+1 < prog->sts.at(index).vals.size()){
            var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(++retIdx).value, scopeStackLoc.size());

            outAsm << "lea " << selectReg(reg, 8) << ", [rsp + " << v->stackLoc << "]\n\t";

        }else{
            std::cerr << "Error, cannot use address of operator(&) without a value." << std::endl;
            exit(1);
        }
        break;
    }

    case tokenType::mul:{ // treating start as a pointer here
        if(retIdx+1 < prog->sts.at(index).vals.size()){
            var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(++retIdx).value, scopeStackLoc.size());
            outAsm << "mov " << selectReg("rbx", v->size) << ", " << selectWord(v->size) <<" [rsp + " << v->stackLoc << "]\n\t";
            
            outAsm << "mov " << selectReg(reg, v->ptrReadBytes) << ", "
                << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        
        }else{
            std::cerr << "Error, cannot use dereference operator(*) without a value." << std::endl;
            exit(1);
        }
    }

    case tokenType::parenOpen:
        retIdx = genExpr(retIdx + 1);
        break;

    default:
        std::cerr << "Error, unknown. (Forgot a semicolon?)" << std::endl;
        exit(1);
        break;
    }

    checkMulDiv:

    if(prog->sts.at(index).vals.size() > retIdx + 1 && 
        (prog->sts.at(index).vals.at(retIdx + 1).type == tokenType::mul ||
        prog->sts.at(index).vals.at(retIdx + 1).type == tokenType::div))
    {
        bool isRax = true;
        if(std::string("rax") != reg){
            outAsm << "mov rax, " << reg << "\n\t";
            isRax = false; 
        }
        retIdx = genMulDiv(retIdx + 1);
        if(!isRax){
            outAsm << "mov " << reg << ", rax\n\t";
        }
    }
    return retIdx;
}

inline void genAsm::genExit()
{
    if(prog->sts.at(index).vals.at(1).type == tokenType::intLit && prog->sts.at(index).vals.size() == 2){
        outAsm << "mov rax, 60\n\tmov rdi, " << prog->sts.at(index).vals.at(1).value << "\n\tsyscall\n\t";
    }else{
        genExpr(1);
        outAsm << "mov rax, 60\n\t";
        outAsm << "syscall\n\t";
    }
}
