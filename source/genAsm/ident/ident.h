#pragma once

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

    case tokenType::percentEq:
        outAsm << "mov rdx, 0\n\t";

        outAsm << "mov " << selectReg("rax", v->size) << ", " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "]\n\t";

        outAsm << "div rdi\n\t";
        
        outAsm << "mov " << selectWord(v->size) <<
            " [rsp + " << (int)(v->stackLoc) << "], " <<
            selectReg("rdx", v->size) << "\n\t";
        break;

    default:
        break;
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
}
