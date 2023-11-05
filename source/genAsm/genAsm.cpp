#include "genAsm.h"
#include "../macros/macros.h"

genAsm::genAsm(const node::program* prog)
{
    this->prog = prog;

    outAsm << "bits 64\n\nsection .text\n\tglobal _start\n\n_start:\n\t";

    for(index = 0; index < prog->sts.size(); index++)
    {
        switch (prog->sts.at(index).vals.at(0).type)
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

        case tokenType::curlyOpen:
        case tokenType::curlyClose:
            genCurly();
            break;

        case tokenType::pp:
        case tokenType::mm:
            genPreIncDec(0);
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

    case tokenType::ident:{

        var* v = (var*)varAccessible(&(prog->sts.at(index).vals.at(retIdx).value), scopeStackLoc.size());

        outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "]\n\t";

        break;
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

void* genAsm::varInScope(const std::string *varName, int scope)
{
    for(auto itr = vars.find(*varName); itr != vars.end(); ++itr){
        if(itr->first != *varName){
            break;
        }

        if(itr->second.scope == scope){
            return &(itr->second);
        }
    }
    return nullptr;
}

void *genAsm::varAccessible(const std::string *varName, int scope)
{
    var* v = nullptr;
    for(auto itr = vars.find(*varName); itr != vars.end(); ++itr){
        if(itr->first != *varName){
            break;
        }

        if(!v || (v && v->scope < itr->second.scope)){
            v = &(itr->second);
        }
    }

    if(!v){
        std::cerr << "Error, identifier '" << *varName << "' is undeclared or not declared in this scope." << std::endl;
        exit(1);
    }
    return v;
}

void genAsm::push(const char *reg, int size, const char *word)
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
        ss << reg; 
        break;

    case 4:
        ss << 'e' << reg[1] << reg[2];
        break;

    case 2:
        ss << reg[1] << "i";
        break;

    case 1:
        if(reg[2] == 'x'){
            ss << reg[1] << 'l';
        }else if(reg[2] == 'i'){
            ss << reg[1] << reg[2] << 'l';
        }
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
    if(prog->sts.at(index).vals.at(1).type == tokenType::intLit && prog->sts.at(index).vals.size() == 2){
        outAsm << "mov rax, 60\n\tmov rdi, " << prog->sts.at(index).vals.at(1).value << "\n\tsyscall\n\t";
    }else{
        genExpr(1);
        outAsm << "mov rax, 60\n\t";
        outAsm << "syscall\n\t";
    }
}

inline void genAsm::genInt()
{
    if(prog->sts.at(index).vals.size() > 2 &&
        varInScope(&(prog->sts.at(index).vals.at(1).value), scopeStackLoc.size()))
    {
        std::cerr << "Error, variable '" << prog->sts.at(index).vals.at(1).value << "' has already been declared." << std::endl;
        exit(1);
    }

    if(3 < prog->sts.at(index).vals.size() &&
        prog->sts.at(index).vals.at(1).type == tokenType::ident &&
        prog->sts.at(index).vals.at(2).type == tokenType::equal)
    {

        vars.insert({prog->sts.at(index).vals.at(1).value,
            var{.stackLoc = stackLoc, .size = 4, .scope = (int)scopeStackLoc.size()}});

        genExpr(3);
        push("edi", SIZE_INT, "DWORD");

    }else{

        // will be possible in the future tho
        std::cerr << "Error, cannot declare variable without a value." << std::endl;
        exit(1);
    }
}

inline void genAsm::genUpdateIdent()
{
    var* v = (var*)varAccessible(&(prog->sts.at(index).vals.at(0).value), scopeStackLoc.size());

    genExpr(2);

            // the operator, = += -= *= /= 
    switch (prog->sts.at(index).vals.at(1).type)
    {
    case tokenType::equal:
        outAsm << "mov " << selectWord(v->size) << " [rsp + " <<
            (int)(v->stackLoc) << "], " <<
            selectReg("rdi", v->size) << "\n\t";
        break;

    case tokenType::pp:
    case tokenType::mm:
        genPostIncDec(0);
        break;

    case tokenType::addEq:
        outAsm << "add " << selectWord(v->size) << " [rsp + " <<
            (int)(v->stackLoc) << "], " <<
            selectReg("rdi", v->size) << "\n\t";
        break;
    
    case tokenType::subEq:
        outAsm << "sub " << selectWord(v->size) << " [rsp + " <<
            (int)(v->stackLoc) << "], " <<
            selectReg("rdi", v->size) << "\n\t";
        break;
    
    case tokenType::mulEq:
        outAsm << "mov rax, rdi\n\t";

        outAsm << "mul " << selectWord(v->size) << " [rsp + " <<
            (int)(v->stackLoc) << "]\n\t";

        outAsm << "mov " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "], " << 
            selectReg("rax", v->size) << "\n\t";
        break;

    case tokenType::divEq:
        outAsm << "mov rdx, 0\n\t";

        outAsm << "mov " << selectReg("rax", v->size) << ", " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "]\n\t";

        outAsm << "div rdi\n\t";
        
        outAsm << "mov " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "], " <<
            selectReg("rax", v->size) << "\n\t";
        break;

    default:
        break;
    }
}

inline void genAsm::genCurly()
{
    if(prog->sts.at(index).vals.at(0).type == tokenType::curlyOpen){

        scopeStackLoc.push_back(stackLoc);

    }else if(prog->sts.at(index).vals.at(0).type == tokenType::curlyClose){

        stackLoc = scopeStackLoc.at(scopeStackLoc.size() - 1);
        scopeStackLoc.pop_back();

        for(std::map<std::string, var>::iterator itr = vars.begin(); itr != vars.end();){
            if(itr->second.scope > scopeStackLoc.size()){
                vars.erase(itr++);
            }else{
                ++itr;
            }
        }

    }
}

inline void genAsm::genPreIncDec(int idx, const char* reg)
{
    var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx + 1).value, scopeStackLoc.size());
    if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
        outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
        outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    }

    if(reg){
        outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    }
    
    //if(idx == 0){
    //    v = (var*)varAccessible(&(prog->sts.at(index).vals.at(0).value), scopeStackLoc.size());
    //    
    //    if(prog->sts.at(index).key.type == tokenType::pp){
    //        outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //    }else if(prog->sts.at(index).key.type == tokenType::mm){
    //        outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //    }
    //}else{
    //    v = (var*)varAccessible(&(prog->sts.at(index).vals.at(idx + 1).value), scopeStackLoc.size());
//
    //    if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
    //        outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //    }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
    //        outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //    }
    //}
//
    //if(reg){
    //    outAsm << "mov " << selectReg(reg, v->size) << ", " <<
    //        selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //}
}

inline void genAsm::genPostIncDec(int idx, const char* reg)
{
    var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());

    if(reg){
        outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    }

    if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::pp){
        outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    }else if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::mm){
        outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    }
    
    //if(idx == 0){
    //    v = (var*)varAccessible(&prog->sts.at(index).key.value, scopeStackLoc.size());
    //}else{
    //    v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());
    //    ++idx;
    //}
//
    //if(reg){
    //    outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //}
//
    //if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
    //    outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //
    //}else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
    //    outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
    //}

}
