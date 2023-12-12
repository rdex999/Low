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
            retIdx = genExpr(index, argsIdxs.at(i)).retIdx;
            eType exprType = getType(index, argsIdxs.at(i));
            if(exprType.type == (tokenType)0){
                std::cerr << "Error, unknown expresion type. Try casting to something." << std::endl;
                exit(1);
            }
            if(exprType.type == tokenType::_float){
                outAsm << "sub rsp, 8\n\t";
                outAsm << "movsd [rsp], xmm0\n\t";
            }else{
                outAsm << "push rax\n\t";
            }
        }
    }

    outAsm << "call " << prog->sts.at(index).vals.at(idx).value << "\n\t";
    return retIdx;
}