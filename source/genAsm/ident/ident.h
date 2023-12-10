#pragma once

inline void genAsm::genUpdateIdent()
{
    var* v = nullptr;
    int identIdx = -1;
    int operatorIdx = -1;
    bool ptr = false;

    for(int i=0; i<prog->sts.at(index).vals.size(); ++i){
        if(prog->sts.at(index).vals.at(i).type >= tokenType::equal && // assignment operator
            prog->sts.at(index).vals.at(i).type <= tokenType::mm)
        {
            operatorIdx = i;
            if(prog->sts.at(index).vals.at(i).type != tokenType::pp &&
                prog->sts.at(index).vals.at(i).type != tokenType::mm)
            {
                genExpr(index, i+1);
            }
            break;
        }
        if(prog->sts.at(index).vals.at(i).type == tokenType::ident){

            if(i+1 < prog->sts.at(index).vals.size() &&
                prog->sts.at(index).vals.at(i+1).type == tokenType::parenOpen) // function call
            {
                genFunctionCall(i);
                return;
            }
            if(identIdx == -1){
                identIdx = i;
                v = (var*)varAccessible(&prog->sts.at(index).vals.at(i).value, scopeStackLoc.size());
                if((i+1 < prog->sts.at(index).vals.size() &&    // index operator []
                    prog->sts.at(index).vals.at(i+1).type == tokenType::bracketOpen) || 
                    (i-1 >= 0 && prog->sts.at(index).vals.at(i-1).type == tokenType::mul)) // pointer
                {
                    ptr = true;
                    i = genSingle(i, "rbx", index, false, false);
                    push("rbx", 8);
                }
            }
        }
    }
    if(operatorIdx == -1){
        std::cerr << "Error, identifier '" << prog->sts.at(index).vals.at(identIdx).value << "' has not effect on this line." << std::endl;
        exit(1);
    }
    if(!v){
        std::cerr << "Error, cannot change an rvalue." << std::endl;
        exit(1);
    }
    if(ptr){
        pop("rbx", 8);
    }

            // the operator, = += -= *= /= 
    switch (prog->sts.at(index).vals.at(operatorIdx).type)
    {
    case tokenType::equal:
        if(v->type == tokenType::_float && (v->ptrReadBytes == -1 || ptr)){
            if(ptr){
                outAsm << "movss " << selectWord(v->ptrReadBytes) << " [rbx], xmm0\n\t";
            }else{
                outAsm << "movss " << selectWord(v->size) << " [rbp - " <<
                    (int)(v->stackLoc) << "], xmm0\n\t";
            }
        }else{
            if(ptr){
                outAsm << "mov " << selectWord(v->ptrReadBytes) << " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";
            }else{
                outAsm << "mov " << selectWord(v->size) << " [rbp - " << (int)(v->stackLoc) << "], " <<
                    selectReg("rax", v->size) << "\n\t";
            }
        }
        break;

    case tokenType::pp:
        if(v->type == tokenType::_float){
            if(ptr){
                outAsm << "movss xmm0, [rbx]\n\t";
                outAsm << "movss xmm1, [f32One]\n\t";
                outAsm << "addss xmm0, xmm1\n\t";
                outAsm << "movss [rbx], xmm0\n\t";
            }else{
                genPostIncDec(identIdx);
            }
        }else{
            if(ptr){
                outAsm << "inc " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }else{
                genPostIncDec(identIdx);
            }
        }
        break; 
    case tokenType::mm:
        if(ptr){
            outAsm << "movss xmm0, [rbx]\n\t";
            outAsm << "movss xmm1, [f32One]\n\t";
            outAsm << "subss xmm0, xmm1\n\t";
            outAsm << "movss [rbx], xmm0\n\t";
        }else{
            genPostIncDec(identIdx);
        }
        break;

    case tokenType::addEq:
        if(v->type == tokenType::_float && v->ptrReadBytes == -1){
            if(ptr){
                outAsm << "movss xmm1, [rbx]\n\t";
                outAsm << "addss xmm1, xmm0\n\t";
                outAsm << "movss [rbx], xmm1\n\t";
            }else{
                outAsm << "movss xmm1, [rbp - " << v->stackLoc << "]\n\t";
                outAsm << "addss xmm1, xmm0\n\t";
                outAsm << "movss [rbp - " << v->stackLoc << "], xmm1\n\t";
            }
        }else{
            if(ptr){
                outAsm << "add " << selectWord(v->ptrReadBytes) <<
                    " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";
            }else{
                outAsm << "add " << selectWord(v->size) << " [rbp - " <<
                    (int)(v->stackLoc) << "], " <<
                    selectReg("rax", v->size) << "\n\t";
            }
        }
        break;
    
    case tokenType::subEq:
        if(v->type == tokenType::_float && v->ptrReadBytes == -1){
            if(ptr){
                outAsm << "movss xmm1, [rbx]\n\t";
                outAsm << "subss xmm1, xmm0\n\t";
                outAsm << "movss [rbx], xmm1\n\t";
            }else{
                outAsm << "movss xmm1, [rbp - " << v->stackLoc << "]\n\t";
                outAsm << "subss xmm1, xmm0\n\t";
                outAsm << "movss [rbp - " << v->stackLoc << "], xmm1\n\t";
            }
        }else{
            if(ptr){
                outAsm << "sub " << selectWord(v->ptrReadBytes) <<
                    " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";
            }else{
                outAsm << "sub " << selectWord(v->size) << " [rbp - " <<
                    (int)(v->stackLoc) << "], " <<
                    selectReg("rax", v->size) << "\n\t";
            }
        }
        break;
    
    case tokenType::mulEq:
        if(v->type == tokenType::_float && v->ptrReadBytes == -1){
            if(ptr){
                outAsm << "movss xmm1, [rbx]\n\t";
                outAsm << "mulss xmm1, xmm0\n\t";
                outAsm << "movss [rbx], xmm1\n\t";
            }else{
                outAsm << "movss xmm1, [rbp - " << v->stackLoc << "]\n\t";
                outAsm << "mulss xmm1, xmm0\n\t";
                outAsm << "movss [rbp - " << v->stackLoc << "], xmm1\n\t";
            }
        }else{
            if(ptr){
                outAsm << "mul " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";

                outAsm << "mov " << selectWord(v->ptrReadBytes)
                    << " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";
            }else{
                outAsm << "mul " << selectWord(v->size) << " [rbp - " <<
                    (int)(v->stackLoc) << "]\n\t";

                outAsm << "mov " << selectWord(v->size) <<
                    " [rbp - " << (int)(v->stackLoc) << "], " << 
                    selectReg("rax", v->size) << "\n\t";
            }
        }
        break;

    case tokenType::divEq:
        if(v->type == tokenType::_float && v->ptrReadBytes == -1){
            if(ptr){
                outAsm << "movss xmm1, [rbx]\n\t";
                outAsm << "divss xmm1, xmm0\n\t";
                outAsm << "movss [rbx], xmm1\n\t";
            }else{
                outAsm << "movss xmm1, [rbp - " << v->stackLoc << "]\n\t";
                outAsm << "divss xmm1, xmm0\n\t";
                outAsm << "movss [rbp - " << v->stackLoc << "], xmm1\n\t";
            }
        }else{
            outAsm << "xor rdx, rdx\n\t";
            outAsm << "mov rcx, rax\n\t";
            if(ptr){
                outAsm << "mov " << selectReg("rax", v->ptrReadBytes) << ", " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
                outAsm << "div rcx\n\t";
                outAsm << "mov " << selectWord(v->ptrReadBytes) << " [rbx], " << selectReg("rax", v->ptrReadBytes) << "\n\t";
            }else{
                outAsm << "mov " << selectReg("rax", v->size) << ", " << selectWord(v->size) <<
                    " [rbp - " << (int)(v->stackLoc) << "]\n\t";

                outAsm << "div rcx\n\t";
                outAsm << "mov " << selectWord(v->size) << " [rbp - " << (int)(v->stackLoc) << "], " <<
                    selectReg("rax", v->size) << "\n\t";
            }
        } 
        break;

    case tokenType::percentEq:
        if(v->type == tokenType::_float){
            std::cerr << "Error, cannot use modulo operator(%) on floats." << std::endl;
            exit(1);
        }

        outAsm << "xor rdx, rdx\n\t";
        outAsm << "mov rcx, rax\n\t";
        if(ptr){
            outAsm << "mov " << selectReg("rax", v->ptrReadBytes)
                << ", " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";

            outAsm << "div rcx\n\t";
            
            outAsm << "mov " << selectWord(v->ptrReadBytes)
                << " [rbx], " << selectReg("rdx", v->ptrReadBytes) << "\n\t";

        }else{
            outAsm << "mov " << selectReg("rax", v->size) << ", " << selectWord(v->size) <<
                " [rbp - " << (int)(v->stackLoc) << "]\n\t";

            outAsm << "div rcx\n\t";

            outAsm << "mov " << selectWord(v->size) <<
                " [rbp - " << (int)(v->stackLoc) << "], " <<
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

        if(v->type == tokenType::_float){
            outAsm << "movss xmm0, [rbx]\n\t";
            outAsm << "movss xmm1, [f32One]\n\t";
            if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
                outAsm << "addss xmm0, xmm1\n\t";
            }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
                outAsm << "subss xmm0, xmm1\n\t";
            }
            outAsm << "movss [rbx], xmm0\n\t";
            if(reg && reg != (std::string)"xmm0"){
                outAsm << "movss " << reg << ", xmm0\n\t";
            }

        }else{
            if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
                outAsm << "inc " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
                outAsm << "dec " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }
            if(reg){
                outAsm << "mov " << selectReg(reg, v->ptrReadBytes) << ", " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }
        }
        return retIdx;

    }else{
        v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx + 1).value, scopeStackLoc.size());
        if(v->type == tokenType::_float){
            outAsm << "movss xmm0, [rbp - " << v->stackLoc << "]\n\t";
            outAsm << "movss xmm1, [f32One]\n\t";
            if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
                outAsm << "addss xmm0, xmm1\n\t";
            }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
                outAsm << "subss xmm0, xmm1\n\t";
            }
            outAsm << "movss [rbp - " << v->stackLoc << "], xmm0\n\t";
            if(reg && reg != (std::string)"xmm0"){
                outAsm << "movss " << reg << ", xmm0\n\t";
            }
        }else{
            if(prog->sts.at(index).vals.at(idx).type == tokenType::pp){
                outAsm << "inc " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
            }else if(prog->sts.at(index).vals.at(idx).type == tokenType::mm){
                outAsm << "dec " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
            }
            if(reg){
                outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
            }
        }

        return idx + 1;
    }
}

inline int genAsm::genPostIncDec(int idx, const char* reg)
{
    var* v = nullptr;
    if(prog->sts.at(index).vals.at(idx).type == tokenType::mul){
        v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx+1).value, scopeStackLoc.size());

        outAsm << "mov rbx, QWORD [rbp - " << v->stackLoc << "]\n\t";

        if(v->type == tokenType::_float){
            if(reg){
                outAsm << "movss xmm2, [rbx]\n\t";
            }
            outAsm << "movss xmm0, [rbx]\n\t";
            outAsm << "movss xmm1, [f32One]\n\t";
            if(prog->sts.at(index).vals.at(idx + 2).type == tokenType::pp){
                outAsm << "addss xmm0, xmm1\n\t";
            }else if(prog->sts.at(index).vals.at(idx + 2).type == tokenType::mm){
                outAsm << "subss xmm0, xmm1\n\t";
            }
            outAsm << "movss [rbx], xmm0\n\t";
            if(reg){
                outAsm << "movss xmm0, xmm2\n\t";
            }
        }else{
            if(reg){
                outAsm << "mov " << selectReg(reg, v->ptrReadBytes) << ", "
                    << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }
            if(prog->sts.at(index).vals.at(idx + 2).type == tokenType::pp){
                outAsm << "inc " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }else if(prog->sts.at(index).vals.at(idx + 2).type == tokenType::mm){
                outAsm << "dec " << selectWord(v->ptrReadBytes) << " [rbx]\n\t";
            }
        }

        return idx + 2;

    }else{
        v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());

        if(v->type == tokenType::_float){
            if(reg){
                outAsm << "movss " << reg << ", " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
                push(reg, 4, "", "movss");
            }
            outAsm << "movss xmm1, [f32One]\n\t";
            outAsm << "movss xmm0, [rbp - " << v->stackLoc << "]\n\t";
            if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::pp){
                outAsm << "addss xmm0, xmm1\n\t";
            }else if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::mm){
                outAsm << "subss xmm0, xmm1\n\t";
            }
            outAsm << "movss [rbp - " << v->stackLoc << "], xmm0\n\t";
            if(reg){
                pop(reg, 4, "", "movss");
            }
        }else{
            if(reg){
                outAsm << "mov " << selectReg(reg, v->size) << ", " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
            }
            if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::pp){
                outAsm << "inc " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
            }else if(prog->sts.at(index).vals.at(idx + 1).type == tokenType::mm){
                outAsm << "dec " << selectWord(v->size) << " [rbp - " << v->stackLoc << "]\n\t";
            }
        }
    
        return idx + 1; 
    }
}
