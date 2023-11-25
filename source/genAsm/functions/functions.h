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
        for(int i = argsIdxs.size() - 1; i>=0; --i){
            retIdx = genExpr(index, argsIdxs.at(i));
            tokenType exprType = getType(index, argsIdxs.at(i));
            switch (i)
            {
            case 0:
                break;

            case 1:
                outAsm << "mov rsi, rdi\n\t";
                break;
            
            case 2:
                outAsm << "mov rdx, rdi\n\t";
                break;
            
            case 3:
                outAsm << "mov rcx, rdi\n\t";
                break;

            case 4:
                outAsm << "mov r8, rdi\n\t";
                break;

            case 5:
                outAsm << "mov r9, rdi\n\t";
                break;

            default:
                switch (exprType){
                case tokenType::_int:
                    push("edi", 4);
                    break;
                case tokenType::_char:
                    push("dil", 1);
                    break;
                case tokenType::_float:
                    push("xmm0", 8, "", "movss");
                    break;
                case tokenType::dQoute:
                    push("rdi", 8);
                    break;
                
                default:
                    std::cerr << "Error, unknown expresion type." << std::endl;
                    exit(1);
                    break;
                }
                break;
            }
        }
    }

    if(v->stackLocReg){
        outAsm << "lea " << v->stackLocReg << ", [rsp + " << stackLoc << "]\n\t";
    }

    outAsm << "call " << prog->sts.at(index).vals.at(idx).value << "\n\t";
    return retIdx;
}