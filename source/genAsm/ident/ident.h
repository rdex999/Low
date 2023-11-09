#pragma once

inline void genAsm::genUpdateIdent()
{
    var* v = nullptr;
    int identIdx = -1;
    bool ptr = false;

    for(int i=0; i<prog->sts.at(index).vals.size(); ++i){
        if(prog->sts.at(index).vals.at(i).type == tokenType::mul){
            ptr = true;
        }

        if(prog->sts.at(index).vals.at(i).type == tokenType::ident){
            v = (var*)varAccessible(&prog->sts.at(index).vals.at(i).value, scopeStackLoc.size());
            identIdx = i;
            break;
        }
    }

    if(prog->sts.at(index).vals.at(identIdx+1).type != tokenType::pp &&
        prog->sts.at(index).vals.at(identIdx+1).type != tokenType::mm)
    {
        genExpr(identIdx+2);
        if(ptr){
            outAsm << "mov rbx, QWORD [rsp + " << v->stackLoc << "]\n\t";
        }
    }


            // the operator, = += -= *= /= 
    switch (prog->sts.at(index).vals.at(identIdx+1).type)
    {
    case tokenType::equal:
        if(ptr){
            outAsm << "mov " << selectWord(v->ptrReadBytes) << " [rbx], " << selectReg("rdi", v->ptrReadBytes) << "\n\t";
        }else{
            outAsm << "mov " << selectWord(v->size) << " [rsp + " <<
                (int)(v->stackLoc) << "], " <<
                selectReg("rdi", v->size) << "\n\t";
        } 
        break;

    case tokenType::pp:
    case tokenType::mm:
        if(ptr){
            genPostIncDec(identIdx-1);
        }else{
            genPostIncDec(identIdx);
        }
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
    var* v = nullptr;
    if(prog->sts.at(index).vals.at(idx).type == tokenType::mul){
        v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx+1).value, scopeStackLoc.size());

        outAsm << "mov rbx, QWORD [rsp + " << v->stackLoc << "]\n\t";

        if(reg){
            outAsm << "mov " << selectReg(reg, v->ptrReadBytes) << ", "
                << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }

        if(prog->sts.at(index).vals.at(idx + 2).type == tokenType::pp){
            outAsm << "inc " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }else if(prog->sts.at(index).vals.at(idx + 2).type == tokenType::mm){
            outAsm << "dec " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }

    }else{
        v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());
        if(reg){
            outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
        }

        if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::pp){
            outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
        }else if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::mm){
            outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
        }
    }
}
