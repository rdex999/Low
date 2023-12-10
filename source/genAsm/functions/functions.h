#pragma once

inline int genAsm::genFunctionCall(int idx)
{
    int retIdx = idx+2;

    var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());
    if(!v->isFunction){
        std::cerr << "Error, '" << prog->sts.at(index).vals.at(idx).value << "' is not a function." << std::endl;
        exit(1);
    }

    // counts arguments
    int parenCount = 0;
    std::vector<int> argsIdxs; 
    for(int i=idx; i<prog->sts.at(index).vals.size(); ++i){
        if(prog->sts.at(index).vals.at(i).type == tokenType::comma){
            argsIdxs.push_back(i+1);
        }
        if(prog->sts.at(index).vals.at(i).type == tokenType::parenOpen){
            ++parenCount;
            if(i == idx+1 && idx+2 < prog->sts.at(index).vals.size() && prog->sts.at(index).vals.at(idx+2).type != tokenType::parenClose){
                argsIdxs.push_back(i+1);
            }
        }else if(prog->sts.at(index).vals.at(i).type == tokenType::parenClose){
            --parenCount;
            if(!parenCount){
                break;
            }
        }
    }

    // generate arguments
    if(argsIdxs.size()){
        int regularRegistersCount = 0;
        int xmmRegistersCount = 0;
        outAsm << "sub rsp, 8\n\t";
        for(int i = argsIdxs.size() - 1; i>=0; --i){
            retIdx = genExpr(index, argsIdxs.at(i));
            eType exprType = getType(index, argsIdxs.at(i));
            if(exprType.type == (tokenType)0){
                std::cerr << "Error, unknown expresion type. Try casting to something." << std::endl;
                exit(1);
            }
            if(exprType.type == tokenType::_float && exprType.ptrReadBytes == -1){
                outAsm << "sub rsp, 8\n\t";
                outAsm << "movsd [rsp], xmm0\n\t";
            }else{
                outAsm << "push rax\n\t";
            }
            //if(exprType.type == tokenType::_int || exprType.type == tokenType::_char){
            //    switch (regularRegistersCount)
            //    {
            //    case 0:
            //        outAsm << "mov rdi, rax\n\t";
            //        break;
//
            //    case 1:
            //        outAsm << "mov rsi, rax\n\t";
            //        break;
//
            //    case 2:
            //        outAsm << "mov rdx, rax\n\t";
            //        break;
//
            //    case 3:
            //        outAsm << "mov rcx, rax\n\t";
            //        break;
//
            //    case 4:
            //        outAsm << "mov r8, rax\n\t";
            //        break;
//
            //    case 5:
            //        outAsm << "mov r9, rax\n\t";
            //        break;
//
            //    default:
            //        push("rax", 8);
            //        break;
            //    }
            //    ++regularRegistersCount;
            //}else if(exprType.type == tokenType::_float){
            //    if(xmmRegistersCount > 0) {
            //        if(xmmRegistersCount < 6){
            //            outAsm << "movss xmm" << xmmRegistersCount << ", xmm0\n\t";
            //        }else{
            //            push("xmm0", 4, "", "movss");
            //        }
            //    }
            //    ++xmmRegistersCount;
            //}
        }
    }

    outAsm << "call " << prog->sts.at(index).vals.at(idx).value << "\n\t";
    return retIdx;
}