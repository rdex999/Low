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
    outAsm << "\n\n_start:\n\t";
    
    if(lowStdLib){
        addStdLibFunc("printStr");
        addStdLibFunc("printChar", "rsi");
        addStdLibFunc("printInt", "rsi");
        addStdLibFunc("printFloat32", "rsi");
    }


    for(index = 0; index < prog->sts.size(); ++index)
    {
        genStmt();
    }

    outAsm << "mov rax, 60\n\tmov rdi, 0\n\tsyscall";

    finalAsm << secData.str() << secText.str() << outAsm.str();
}

inline void genAsm::addStdLibFunc(const char* funcName, const char* stackReg)
{
    vars.insert({funcName, var{.stackLoc = 0, .size = -1, .scope = 0, .ptrReadBytes = -1,
        .isFunction = true, .isExtern = true, .stackLocReg = stackReg}});

    secText << (std::string)"\n\textern " + funcName;
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
}

int genAsm::genSingle(int idx, const char* reg, size_t stmtIdx, bool checkPostPreIncDec, bool ifPtrGetPValue)
{
    int retIdx = idx;
    int oprSize = -1;
    tokenType type = getType(stmtIdx, idx);
    if(type == (tokenType)0){
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
        outAsm << "mov " << selectReg(reg, 4) << ", " << prog->sts.at(stmtIdx).vals.at(retIdx).value << "\n\t";
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

        outAsm << "mov rdi, " << textName << "\n\t";
        break;
    }

    case tokenType::ident:{

        var* v = (var*)varAccessible(&(prog->sts.at(stmtIdx).vals.at(retIdx).value), scopeStackLoc.size());
        if(oprSize == -1){
            oprSize = v->ptrReadBytes != -1 ? v->ptrReadBytes : v->size;
        }
        if(v->ptrReadBytes == -1){
            if(type == tokenType::_float){
                outAsm << "movss " << reg << ", [rsp + " << (int)(v->stackLoc) << "]\n\t";
            }else{
                outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) <<
                    " [rsp + " << (int)(v->stackLoc) << "]\n\t";
            }
        }else{
            outAsm << "mov " << selectReg(reg, 8) << ", " << selectWord(8) <<
                    " [rsp + " << (int)(v->stackLoc) << "]\n\t";
        }

        break;
    }

    case tokenType::singleAnd:{
        if(retIdx+1 < prog->sts.at(stmtIdx).vals.size()){
            var* v = (var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(++retIdx).value, scopeStackLoc.size());
            if(oprSize == -1){
                oprSize = v->ptrReadBytes != -1 ? v->ptrReadBytes : v->size;
            }

            outAsm << "lea " << selectReg(reg, 8) << ", [rsp + " << v->stackLoc << "]\n\t";

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
                var* v = (var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(++retIdx).value, scopeStackLoc.size());
                if(oprSize != -1){
                    oprSize = v->ptrReadBytes != -1 ? v->ptrReadBytes : v->size;
                }

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
        retIdx = genExpr(stmtIdx, retIdx + 1);
        if((std::string)reg != "rdi"){
            outAsm << "mov " << reg << ", rdi\n\t";
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
        if(type == tokenType::_float){
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
        if(oprSize == -1){
            std::cerr << "Error, must specify data type to use index operator ( [] ).\nNote: You can do this \"*(var+offset)\"" << std::endl;
            exit(1);
        }

        push(reg, 8);
        retIdx = genExpr(stmtIdx, retIdx+2);
        outAsm << "mov rax, rdi\n\t";
        if(oprSize != 1){
            outAsm << "mov rcx, " << oprSize << "\n\t";
            outAsm << "mul rcx\n\t";
        }
        pop(reg, 8);
        outAsm << "add " << reg << ", rax\n\t";
        if(ifPtrGetPValue){
            outAsm << "mov " << selectReg(reg, oprSize) << ", " << selectWord(oprSize) << " [" << reg << "]\n\t";
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
            return -1;
        }
    }
    return -1;
}

inline tokenType genAsm::getType(size_t stmtIdx, int idx)
{
    for(; idx<prog->sts.at(stmtIdx).vals.size(); ++idx){
        switch (prog->sts.at(stmtIdx).vals.at(idx).type)
        {
        case tokenType::ident:
            return ((var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(idx).value, scopeStackLoc.size()))->type;

        case tokenType::intLit:
            return tokenType::_int;

        case tokenType::floatLit:
            return tokenType::_float;

        case tokenType::quote:
            return tokenType::_char;

        case tokenType::dQoute:
            return tokenType::dQoute;

        case tokenType::singleAnd:
            return tokenType::ptr;

        default:
            break;
        }
    }

    return (tokenType)0;
}