#include "genAsm.h"
#include "../macros/macros.h"
#include "basicMath/basicMath.h"
#include "vars/vars.h"
#include "stack/stack.h"
#include "registers/registers.h"
#include "genIf/genIf.h"
#include "int/int.h"
#include "char/char.h"
#include "float/float.h"
#include "ident/ident.h"
#include "functions/functions.h"
#include "while/while.h"
#include "for/for.h"

genAsm::genAsm(const node::program* prog, bool lowStdLib)
{
    this->prog = prog;

    secData << "bits 64\n\nsection .data\n\tf32One: dd 1.0";
    secText << "\n\nsection .text\n\tglobal _start";
    mainInit();

    if(lowStdLib){
        addStdLibFunc("printStr");
        addStdLibFunc("printChar");
        addStdLibFunc("printInt");
        addStdLibFunc("printFloat32");
    }


    for(index = 0; index < prog->sts.size(); ++index)
    {
        genStmt();
    }

    mainExit();

    finalAsm << secData.str() << secText.str() << outAsm.str();
}

inline void genAsm::addStdLibFunc(const char* funcName)
{
    vars.insert({funcName, var{.stackLoc = 0, .size = -1, .scope = 0, .ptrReadBytes = -1,
        .isFunction = true, .isExtern = true}});

    secText << "\n\textern " << funcName;
}

inline void genAsm::mainInit()
{
    outAsm << "\n\n_start:\n\t";
    outAsm << "push rbp\n\t";
    outAsm << "mov rbp, rsp\n\t";
    outAsm << "sub rsp, " << prog->mainStackSize << "\n\n\t";
}

inline void genAsm::mainExit()
{
    outAsm << "mov rsp, rbp\n\t";
    outAsm << "pop rbp\n\t";
    outAsm << "mov rax, 60\n\t";
    outAsm << "xor rdi, rdi\n\t";
    outAsm << "syscall\n\t";
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

    case tokenType::_float:
        genFloat();
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
    case tokenType::_else:
    case tokenType::elseIf:
        genIf();
        break;

    case tokenType::_while:
        genWhile();
        break;

    case tokenType::_for:
        genFor();
        break;

    case tokenType::lable:
        outAsm << "\r" << prog->sts.at(index).vals.at(0).value << ":\n\t";
        break;

    case tokenType::_goto:
        outAsm << "jmp " << prog->sts.at(index).vals.at(1).value << "\n\t";
        break;

    default:
        break;
    }
    outAsm << "\n\t";
}

int genAsm::genSingle(int idx, const char* reg, size_t stmtIdx, bool checkPostPreIncDec, bool ifPtrGetPValue)
{
    int retIdx = idx;
    eType type = getType(stmtIdx, idx);
    if(type.type == (tokenType)0){
        std::cerr << "Error, no expresion type." << std::endl;
        exit(1);
    }

    if(checkPostPreIncDec && prog->sts.at(stmtIdx).vals.size() > retIdx + 1 &&
        (prog->sts.at(stmtIdx).vals.at(retIdx + 1).type == tokenType::pp ||
        prog->sts.at(stmtIdx).vals.at(retIdx + 1).type == tokenType::mm))
    {
        retIdx = genPostIncDec(retIdx, reg);
        goto checkMulDiv;

    }else if(checkPostPreIncDec && prog->sts.at(stmtIdx).vals.at(retIdx).type == tokenType::pp ||
        prog->sts.at(stmtIdx).vals.at(retIdx).type == tokenType::mm)
    {
        retIdx = genPreIncDec(retIdx, reg);
        goto checkMulDiv;
    }

    switch (prog->sts.at(stmtIdx).vals.at(retIdx).type)
    {
    case tokenType::intLit:
        outAsm << "mov " << reg << ", " << prog->sts.at(stmtIdx).vals.at(retIdx).value << "\n\t";
        break;

    case tokenType::floatLit:{
        std::string floatDataName = createFloat32VarName();
        secData << "\n\t" << floatDataName << ": dd " << prog->sts.at(stmtIdx).vals.at(retIdx).value;
        outAsm << "movss " << reg << ", DWORD [" << floatDataName << "]\n\t";
        break;
    }

    case tokenType::quote:
        outAsm << "mov " << selectReg(reg, 1) << ", " << prog->sts.at(stmtIdx).vals.at(retIdx).value << "\n\t";
        break;

    case tokenType::dQoute:{
        std::string textName = createTextVarName();
        secData << "\n\t" << textName <<
            " db " << handleSpecialChar(&prog->sts.at(stmtIdx).vals.at(retIdx).value) << ", 0";

        outAsm << "mov " << reg << ", " << textName << "\n\t";
        break;
    }

    case tokenType::ident:{

        var* v = (var*)varAccessible(&(prog->sts.at(stmtIdx).vals.at(retIdx).value), scopeStackLoc.size());
        if(v->ptrReadBytes == -1){
            if(type.type == tokenType::_float){
                outAsm << "movss " << reg << ", [rbp - " << (int)(v->stackLoc) << "]\n\t";
            }else{
                outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) <<
                    " [rbp - " << (int)(v->stackLoc) << "]\n\t";
            }
        }else{
            if(type.type == tokenType::_float){
                outAsm << "mov rax, [rbp - " << v->stackLoc << "]\n\t";
            }else{
                outAsm << "mov " << selectReg(reg, 8) << ", " << selectWord(8) <<
                        " [rbp - " << (int)(v->stackLoc) << "]\n\t";
            } 
        }

        break;
    }

    case tokenType::singleAnd:{
        if(retIdx+1 < prog->sts.at(stmtIdx).vals.size()){
            var* v = (var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(++retIdx).value, scopeStackLoc.size());
            outAsm << "lea " << selectReg(reg, 8) << ", [rbp - " << v->stackLoc << "]\n\t";

        }else{
            std::cerr << "Error, cannot use address of operator(&) without a value." << std::endl;
            exit(1);
        }
        break;
    }

    case tokenType::mul:{ // treating start as a pointer here
        if(retIdx+1 < prog->sts.at(stmtIdx).vals.size()){

            if(retIdx+2 < prog->sts.at(stmtIdx).vals.size() &&
                (prog->sts.at(stmtIdx).vals.at(retIdx+2).type == tokenType::mm ||
                prog->sts.at(stmtIdx).vals.at(retIdx+2).type == tokenType::pp))
            {
                retIdx = genPostIncDec(retIdx, reg);

            }else{
                retIdx = genSingle(retIdx+1, "rax", stmtIdx);

                if(type.type == tokenType::_float){
                    outAsm << "movss " << reg << ", [rax]\n\t";
                }else{
                    outAsm << "mov " << selectReg(reg, type.ptrReadBytes) << ", [rax]\n\t";
                }
            }

            break;
        
        }else{
            std::cerr << "Error, cannot use dereference operator(*) without a value." << std::endl;
            exit(1);
        }
    }

    case tokenType::parenOpen:
        retIdx = genExpr(stmtIdx, retIdx + 1);
        if((std::string)reg != "rax"){
            outAsm << "mov " << reg << ", rax\n\t";
        }
        break;

    default:
        std::cerr << "Error, unknown. (Forgot a semicolon?)" << std::endl;
        exit(1);
        break;
    }

    checkMulDiv:

    if(prog->sts.at(stmtIdx).vals.size() > retIdx + 1 && 
        (prog->sts.at(stmtIdx).vals.at(retIdx + 1).type == tokenType::mul ||
        prog->sts.at(stmtIdx).vals.at(retIdx + 1).type == tokenType::div))
    {
        if(type.type == tokenType::_float){
            bool isXmm0 = true;
            if((std::string)"xmm0" != reg){
                outAsm << "movss xmm0, " << reg << "\n\t";
                isXmm0 = false;
            }
            retIdx = genMulDiv(retIdx + 1, stmtIdx, tokenType::_float);
            if(!isXmm0){
                outAsm << "movss " << reg << ", xmm0\n\t";
            }
        
        }else{
            bool isRax = true;
            if(std::string("rax") != reg){
                outAsm << "mov rax, " << reg << "\n\t";
                isRax = false; 
            }
            retIdx = genMulDiv(retIdx + 1, stmtIdx, tokenType::_int);
            if(!isRax){
                outAsm << "mov " << reg << ", rax\n\t";
            }
        }
    }

    if(retIdx + 1 < prog->sts.at(stmtIdx).vals.size() &&
        prog->sts.at(stmtIdx).vals.at(retIdx+1).type == tokenType::bracketOpen)
    {
        if(type.ptrReadBytes == -1){
            std::cerr << "Error, must specify data type to use index operator ( [] ).\nNote: You can do this \"*(var+offset)\"" << std::endl;
            exit(1);
        }

        if(type.type == tokenType::_float){
            push("rax", 8);
            retIdx = genExpr(stmtIdx, retIdx+2);
            outAsm << "mov rcx, " << type.ptrReadBytes << "\n\t";
            outAsm << "mul rcx\n\t";
            pop("rbx", 8);
            outAsm << "add rbx, rax\n\t";
            if(ifPtrGetPValue){
                outAsm << "movss " << reg << ", [rbx]\n\t";
            }else if((std::string)"rbx" != reg){
                outAsm << "mov " << reg << ", rbx\n\t";
            }
        }else{
            push(reg, 8);
            retIdx = genExpr(stmtIdx, retIdx+2);
            outAsm << "mov rcx, " << type.ptrReadBytes << "\n\t";
            outAsm << "mul rcx\n\t";
            pop("rbx", 8);
            outAsm << "add rbx, rax\n\t";
            if(ifPtrGetPValue){
                outAsm << "mov " << selectReg(reg, type.ptrReadBytes) << ", " << selectWord(type.ptrReadBytes) << " [rbx]\n\t";
            }else if((std::string)"rbx" != reg){
                outAsm << "mov " << reg << ", rbx\n\t";
            }
        }
    }
    return retIdx;
}

inline std::string genAsm::createTextVarName()
{
    return std::format("text{}", textVarCount++);
}

inline std::string genAsm::createFloat32VarName()
{
    return std::format("f32_{}", textFloat32Count++);
}

inline void genAsm::genExit()
{
    if(prog->sts.at(index).vals.at(1).type == tokenType::intLit && prog->sts.at(index).vals.size() == 2){
        outAsm << "mov rax, 60\n\tmov rdi, " << prog->sts.at(index).vals.at(1).value << "\n\tsyscall\n\t";
    }else{
        genExpr(index, 1);
        outAsm << "mov rdi, rax\n\t";
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

            case '"': 
                out.replace(i, 2, "\", 34, \""); 
                break;

            case 'b':
                out.replace(i, 2, "\", 8, \""); 
                break;

            case 'f':
                out.replace(i, 2, "\", 12, \""); 
                break;

            case '0':
                out.replace(i, 2, "\", 0, \""); 
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

inline int genAsm::getOprSize(size_t stmtIdx, int idx)
{
    for(; idx < prog->sts.at(stmtIdx).vals.size(); ++idx){
        switch (prog->sts.at(stmtIdx).vals.at(idx).type)
        {
        case tokenType::ident:
            return ((var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(idx).value, scopeStackLoc.size()))->ptrReadBytes;
            break;

        default:
            break; 
        }
    }
    return -1;
}

inline eType genAsm::getType(size_t stmtIdx, int idx)
{
    for(; idx<prog->sts.at(stmtIdx).vals.size(); ++idx){
        switch (prog->sts.at(stmtIdx).vals.at(idx).type)
        {
        case tokenType::ident:{
            var* v = (var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(idx).value, scopeStackLoc.size());
            return eType{.type = v->type, .ptrReadBytes = v->ptrReadBytes};
        }

        case tokenType::intLit:
            return eType{.type = tokenType::_int};

        case tokenType::floatLit:
            return eType{.type = tokenType::_float};

        case tokenType::quote:
            return eType{.type = tokenType::_char};

        case tokenType::dQoute:
            return eType{.type = tokenType::dQoute};

        case tokenType::singleAnd:
            return eType{.type = tokenType::ptr};

        default:
            break;
        }
    }

    return eType{.type = (tokenType)0};
}