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
#include "functions/functions.h"

genAsm::genAsm(const node::program* prog, bool lowStdLib)
{
    this->prog = prog;

    secData << "bits 64\n\nsection .data";
    secText << "\n\nsection .text\n\tglobal _start";
    outAsm << "\n\n_start:\n\t";
    
    if(lowStdLib){
        vars.insert({"printStr", var{.stackLoc = 0, .size = -1,
            .scope = 0, .ptrReadBytes = -1, .isFunction = true, .isExtern = true}});

        secText << "\n\textern printStr";
    }


    for(index = 0; index < prog->sts.size(); index++)
    {
        genStmt();
    }

    outAsm << "mov rax, 60\n\tmov rdi, 0\n\tsyscall";

    finalAsm << secData.str() << secText.str() << outAsm.str();
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
    case tokenType::mul:
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
        retIdx = genPostIncDec(retIdx, reg);
        goto checkMulDiv;

    }else if(prog->sts.at(index).vals.at(retIdx).type == tokenType::pp ||
        prog->sts.at(index).vals.at(retIdx).type == tokenType::mm)
    {
        retIdx = genPreIncDec(retIdx, reg);
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

    case tokenType::dQoute:{
        std::string textName = createTextVarName();
        secData << "\n\t" << textName <<
            " db " << handleSpecialChar(&prog->sts.at(index).vals.at(retIdx).value) << ", 0";

        outAsm << "mov rdi, " << textName << "\n\t";
        break;
    }

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

            if(retIdx+2 < prog->sts.at(index).vals.size() &&
                (prog->sts.at(index).vals.at(retIdx+2).type == tokenType::mm ||
                prog->sts.at(index).vals.at(retIdx+2).type == tokenType::pp))
            {
                retIdx = genPostIncDec(retIdx, reg);

            }else{
                var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(++retIdx).value, scopeStackLoc.size());
                outAsm << "mov " << selectReg("rbx", v->size) <<
                    ", " << selectWord(v->size) <<" [rsp + " << v->stackLoc << "]\n\t";

                outAsm << "mov " << selectReg(reg, v->ptrReadBytes) << ", "
                    << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }

            break;
        
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

inline std::string genAsm::createTextVarName()
{
    return std::format("text{}", textVarCount++);
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

inline std::string genAsm::handleSpecialChar(const std::string* str)
{
    std::string out = *str;
    for(int i=0; i<out.size(); ++i){
        if(out.at(i) == '\\' && i+1 < out.size()){
            switch (out.at(i+1))
            {
            case 'n':
                out.replace(i, 2, "\", 10, \"");
                break;
            
            case 'r':
                out.replace(i, 2, "\", 13, \""); 
                break;
            
            case 't':
                out.replace(i, 2, "\", 9, \""); 
                break;

            default:
                out.erase(i, 1);
                ++i;
                break;
            }
        }
    }
    return out;
}