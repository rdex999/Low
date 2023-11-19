#pragma once

inline int genAsm::genFunctionCall(int idx)
{
    int retIdx = idx+2;

    var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());
    if(!v->isFunction){
        std::cerr << "Error, '" << prog->sts.at(index).vals.at(idx).value << "' is not a function." << std::endl;
        exit(1);
    }

    for(int i=0; i<v->params.size(); ++i){
        
        retIdx = genExpr(index, retIdx);
        if(prog->sts.at(index).vals.at(retIdx).type == tokenType::parenClose && i < v->params.size() - 1){
            std::cerr << "Error, too few arguments to function call. '" <<
                prog->sts.at(index).vals.at(idx).value << "' takes " << v->params.size() << " arguments." << std::endl;
            exit(1);
        }
        
        if(prog->sts.at(index).vals.at(retIdx).type != tokenType::comma &&
            prog->sts.at(index).vals.at(retIdx).type != tokenType::parenClose)
        {
            std::cerr << "Error, expected comma (,)" << std::endl;
            exit(1);
        }
        ++retIdx;
    }

    if(v->stackLocReg){
        outAsm << "lea " << v->stackLocReg << ", [rsp + " << stackLoc << "]\n\t";
    }

    outAsm << "call " << prog->sts.at(index).vals.at(idx).value << "\n\t";
    return retIdx;
}