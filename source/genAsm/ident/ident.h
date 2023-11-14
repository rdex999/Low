#pragma once

inline void genAsm::genUpdateIdent()
{
    var* v = nullptr;
    int identIdx = -1;
    bool ptr = false;

    if(prog->sts.at(index).vals.at(0).type == tokenType::mul){
        ptr = true;
        identIdx = genSingle(1, "rbx", index);
        if(prog->sts.at(index).vals.at(identIdx+1).type != tokenType::mm  &&
            prog->sts.at(index).vals.at(identIdx+1).type != tokenType::pp)
        {
            push("rbx", 8);
            genExpr(index, identIdx+2);
            pop("rbx", 8);
        }

    }else if(prog->sts.at(index).vals.at(0).type == tokenType::ident){
        if(prog->sts.at(index).vals.at(1).type == tokenType::parenOpen){ // if is a function call
            genFunctionCall(0);
            return;
        }

        identIdx = 0;
        if(prog->sts.at(index).vals.at(1).type != tokenType::mm  &&
            prog->sts.at(index).vals.at(1).type != tokenType::pp)
        {
            genExpr(index, 2);
        }
    }
 
    for(int i=0; i<prog->sts.at(index).vals.size(); ++i){
        if(prog->sts.at(index).vals.at(i).type == tokenType::ident){
            v = (var*)varAccessible(&prog->sts.at(index).vals.at(i).value, scopeStackLoc.size());
            break;
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
        if(ptr){
            outAsm << "inc " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }else{
            genPostIncDec(identIdx);
        }
        break; 
    case tokenType::mm:
        if(ptr){
            outAsm << "dec " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }else{
            genPostIncDec(identIdx);
        }
        break;

    case tokenType::addEq:
        if(ptr){
            outAsm << "add " << selectWord(v->ptrReadBytes) <<
                " [rbx], " << selectReg("rdi", v->ptrReadBytes) << "\n\t";
        }else{
            outAsm << "add " << selectWord(v->size) << " [rsp + " <<
                (int)(v->stackLoc) << "], " <<
                selectReg("rdi", v->size) << "\n\t";
        } 
        break;
    
    case tokenType::subEq:
        if(ptr){
            outAsm << "sub " << selectWord(v->ptrReadBytes) <<
                " [rbx], " << selectReg("rdi", v->ptrReadBytes) << "\n\t";
        }else{
            outAsm << "sub " << selectWord(v->size) << " [rsp + " <<
                (int)(v->stackLoc) << "], " <<
                selectReg("rdi", v->size) << "\n\t";
        } 
        break;
    
    case tokenType::mulEq:
        outAsm << "mov rax, rdi\n\t";

        if(ptr){
            outAsm << "mul " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            
            outAsm << "mov " << selectWord(v->ptrReadBytes)
                << " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";
        }else{
            outAsm << "mul " << selectWord(v->size) << " [rsp + " <<
                (int)(v->stackLoc) << "]\n\t";

            outAsm << "mov " << selectWord(v->size) <<
                " [rsp + " << (int)(v->stackLoc) << "], " << 
                selectReg("rax", v->size) << "\n\t";
        } 
        break;

    case tokenType::divEq:
        outAsm << "xor rdx, rdx\n\t";

        if(ptr){
            outAsm << "mov " << selectReg("rax", v->ptrReadBytes) << ", " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";

            outAsm << "div rdi\n\t";

            outAsm << "mov " << selectWord(v->ptrReadBytes) << " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";

        }else{
            outAsm << "mov " << selectReg("rax", v->size) << ", " << selectWord(v->size) <<
                " [rsp + " << (int)(v->stackLoc) << "]\n\t";
            
            outAsm << "div rdi\n\t";

            outAsm << "mov " << selectWord(v->size) <<
                " [rsp + " << (int)(v->stackLoc) << "], " <<
                selectReg("rax", v->size) << "\n\t";
        }
        break;

    case tokenType::percentEq:
        outAsm << "xor rdx, rdx\n\t";

        if(ptr){
            outAsm << "mov " << selectReg("rax", v->ptrReadBytes)
                << ", " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";

            outAsm << "div rdi\n\t";
            
            outAsm << "mov " << selectWord(v->ptrReadBytes)
                << " [rbx], " << selectReg("rdx", v->ptrReadBytes) << "\n\t";

        }else{
            outAsm << "mov " << selectReg("rax", v->size) << ", " << selectWord(v->size) <<
                " [rsp + " << (int)(v->stackLoc) << "]\n\t";

            outAsm << "div rdi\n\t";

            outAsm << "mov " << selectWord(v->size) <<
                " [rsp + " << (int)(v->stackLoc) << "], " <<
                selectReg("rdx", v->size) << "\n\t";
        } 
        break;

    default:
        break;
    }
}

inline int genAsm::genPreIncDec(int idx, const char* reg)
{
    var* v = nullptr;
    int retIdx = idx;
    
    if(prog->sts.at(index).vals.at(idx+1).type == tokenType::mul){
        
        for(int i=idx; i<prog->sts.at(index).vals.size(); ++i){
            if(prog->sts.at(index).vals.at(i).type == tokenType::ident){
                v = (var*)varAccessible(&prog->sts.at(index).vals.at(i).value, scopeStackLoc.size());
                break;
            }
        }

        retIdx = genSingle(idx+2, "rbx", index);

        if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
            outAsm << "inc " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
            outAsm << "dec " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }

        if(reg){
            outAsm << "mov " << selectReg(reg, v->ptrReadBytes) << ", " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
        }
        return retIdx;

    }else{
        v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx + 1).value, scopeStackLoc.size());
        if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
            outAsm << "inc " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
        }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
            outAsm << "dec " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
        }

        if(reg){
            outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rsp + " << v->stackLoc << "]\n\t";
        }

        return idx + 1;
    }
}

inline int genAsm::genPostIncDec(int idx, const char* reg)
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

        return idx + 2;

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
    
        return idx + 1; 
    }
}
