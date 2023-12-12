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
        addStdLibFunc("print");
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

exprRes genAsm::genSingle(int idx, const char* reg, size_t stmtIdx, bool checkPostPreIncDec, bool ifPtrGetPValue)
{
    exprRes res = exprRes{.retIdx = idx};
    eType type = getType(stmtIdx, idx);
    if(type.type == (tokenType)0){
        std::cerr << "Error, no expresion type." << std::endl;
        exit(1);
    }

    if(checkPostPreIncDec && prog->sts.at(stmtIdx).vals.size() > res.retIdx + 1 &&
        (prog->sts.at(stmtIdx).vals.at(res.retIdx + 1).type == tokenType::pp ||
        prog->sts.at(stmtIdx).vals.at(res.retIdx + 1).type == tokenType::mm))
    {
        res.retIdx = genPostIncDec(res.retIdx, reg);
        goto checkMulDiv;

    }else if(checkPostPreIncDec && prog->sts.at(stmtIdx).vals.at(res.retIdx).type == tokenType::pp ||
        prog->sts.at(stmtIdx).vals.at(res.retIdx).type == tokenType::mm)
    {
        res.retIdx = genPreIncDec(res.retIdx, reg);
        goto checkMulDiv;
    }

    switch (prog->sts.at(stmtIdx).vals.at(res.retIdx).type)
    {
    case tokenType::intLit:
        outAsm << "mov " << reg << ", " << prog->sts.at(stmtIdx).vals.at(res.retIdx).value << "\n\t";
        break;

    case tokenType::floatLit:{
        std::string floatDataName = createFloat32VarName();
        secData << "\n\t" << floatDataName << ": dd " << prog->sts.at(stmtIdx).vals.at(res.retIdx).value;
        outAsm << "movss xmm0, DWORD [" << floatDataName << "]\n\t";
        break;
    }

    case tokenType::quote:
        outAsm << "mov " << selectReg(reg, 1) << ", " << prog->sts.at(stmtIdx).vals.at(res.retIdx).value << "\n\t";
        break;

    case tokenType::dQoute:{
        std::string textName = createTextVarName();
        secData << "\n\t" << textName <<
            " db " << handleSpecialChar(&prog->sts.at(stmtIdx).vals.at(res.retIdx).value) << ", 0";

        outAsm << "mov " << reg << ", " << textName << "\n\t";
        break;
    }

    case tokenType::ident:{

        var* v = (var*)varAccessible(&(prog->sts.at(stmtIdx).vals.at(res.retIdx).value), scopeStackLoc.size());
        if(v->ptrReadBytes == -1){
            if(type.type == tokenType::_float){
                outAsm << "movss xmm0, [rbp - " << (int)(v->stackLoc) << "]\n\t";
            }else{
                outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) <<
                    " [rbp - " << (int)(v->stackLoc) << "]\n\t";
            }
        }else{
            if(type.type == tokenType::_float){
                outAsm << "mov rax, [rbp - " << v->stackLoc << "]\n\t";
            }else{
                outAsm << "mov rax, " << selectWord(8) <<
                        " [rbp - " << (int)(v->stackLoc) << "]\n\t";
            } 
        }

        break;
    }

    case tokenType::singleAnd:{
        if(res.retIdx+1 < prog->sts.at(stmtIdx).vals.size()){
            var* v = (var*)varAccessible(&prog->sts.at(stmtIdx).vals.at(++res.retIdx).value, scopeStackLoc.size());
            outAsm << "lea " << reg << ", [rbp - " << v->stackLoc << "]\n\t";

        }else{
            std::cerr << "Error, cannot use address of operator(&) without a value." << std::endl;
            exit(1);
        }
        break;
    }

    case tokenType::mul:{ // treating start as a pointer here
        if(res.retIdx+1 < prog->sts.at(stmtIdx).vals.size()){

            if(res.retIdx+2 < prog->sts.at(stmtIdx).vals.size() &&
                (prog->sts.at(stmtIdx).vals.at(res.retIdx+2).type == tokenType::mm ||
                prog->sts.at(stmtIdx).vals.at(res.retIdx+2).type == tokenType::pp))
            {
                res.retIdx = genPostIncDec(res.retIdx, reg);

            }else{
                res.retIdx = genSingle(res.retIdx+1, "rax", stmtIdx).retIdx;

                if(type.type == tokenType::_float){
                    outAsm << "movss xmm0, [rax]\n\t";
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
        res.retIdx = genExpr(stmtIdx, res.retIdx + 1).retIdx;
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

    if(prog->sts.at(stmtIdx).vals.size() > res.retIdx + 1 && 
        (prog->sts.at(stmtIdx).vals.at(res.retIdx + 1).type == tokenType::mul ||
        prog->sts.at(stmtIdx).vals.at(res.retIdx + 1).type == tokenType::div))
    {
        if(type.type == tokenType::_float){
            res.retIdx = genMulDiv(res.retIdx + 1, stmtIdx, tokenType::_float);
        
        }else{
            bool isRax = true;
            if(std::string("rax") != reg){
                outAsm << "mov rax, " << reg << "\n\t";
                isRax = false; 
            }
            res.retIdx = genMulDiv(res.retIdx + 1, stmtIdx, tokenType::_int);
            if(!isRax){
                outAsm << "mov " << reg << ", rax\n\t";
            }
        }
    }

    if(res.retIdx + 1 < prog->sts.at(stmtIdx).vals.size() &&
        prog->sts.at(stmtIdx).vals.at(res.retIdx+1).type == tokenType::bracketOpen)
    {
        if(type.ptrReadBytes == -1){
            std::cerr << "Error, must specify data type to use index operator ( [] ).\nNote: You can do this \"*(var+offset)\"" << std::endl;
            exit(1);
        }

        if(res.retIdx + 3 < prog->sts.at(stmtIdx).vals.size()){
            if(prog->sts.at(stmtIdx).vals.at(res.retIdx+2).type == tokenType::intLit &&
                prog->sts.at(stmtIdx).vals.at(res.retIdx+3).type == tokenType::bracketClose)
            {
                if(ifPtrGetPValue){
                    if(type.type == tokenType::_float){
                        outAsm << "movss xmm0, [rax + " << type.ptrReadBytes << " * " << prog->sts.at(stmtIdx).vals.at(res.retIdx+2).value << "]\n\t";
                    }else{
                        outAsm << "mov " << selectReg(reg, type.ptrReadBytes) << ", [rax + " << type.ptrReadBytes << " * " << prog->sts.at(stmtIdx).vals.at(res.retIdx+2).value << "]\n\t";
                    }
                }else{
                    outAsm << "lea " << reg << ", [rax + " << type.ptrReadBytes << " * " << prog->sts.at(stmtIdx).vals.at(res.retIdx+2).value << "]\n\t";
                }
                res.retIdx += 3; 
            }else{
                if(type.type == tokenType::_float){
                    push("rax", 8);
                    res.retIdx = genExpr(stmtIdx, res.retIdx+2).retIdx;
                    outAsm << "mov rcx, " << type.ptrReadBytes << "\n\t";
                    outAsm << "mul rcx\n\t";
                    pop("rbx", 8);
                    outAsm << "add rbx, rax\n\t";
                    if(ifPtrGetPValue){
                        outAsm << "movss xmm0, [rbx]\n\t";
                    }else if((std::string)"rbx" != reg){
                        outAsm << "mov " << reg << ", rbx\n\t";
                    }
                }else{
                    push("rax", 8);
                    res.retIdx = genExpr(stmtIdx, res.retIdx+2).retIdx;
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
        }else{
            std::cerr << "Error, expected closing bracket. ( ] )" << std::endl;
            exit(1);
        }

    }
    return res;
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
                ++i;
                break;
            
            case 'r':
                out.replace(i, 2, "\", 13, \""); 
                ++i;
                break;
            
            case 't':
                out.replace(i, 2, "\", 9, \""); 
                ++i;
                break;

            case '"': 
                out.replace(i, 2, "\", 34, \""); 
                ++i;
                break;

            case 'b':
                out.replace(i, 2, "\", 8, \""); 
                ++i;
                break;

            case 'f':
                out.replace(i, 2, "\", 12, \""); 
                ++i;
                break;

            case '0':
                out.replace(i, 2, "\", 0, \""); 
                ++i;
                break;

            case '%':
                ++i;
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