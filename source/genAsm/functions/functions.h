#pragma once

inline int genAsm::genFunctionCall(int idx)
{
    int retIdx = idx;

    var* v = (var*)varAccessible(&prog->sts.at(index).vals.at(idx).value, scopeStackLoc.size());
    if(!v->isFunction){
        std::cerr << "Error, '" << prog->sts.at(index).vals.at(idx).value << "' is not a function." << std::endl;
        exit(1);
    }

    retIdx = genExpr(idx+2);

    if(v->stackLocReg){
        outAsm << "lea " << v->stackLocReg << ", [rsp + " << stackLoc << "]\n\t";
    }

    outAsm << "call " << prog->sts.at(index).vals.at(idx).value << "\n\t";
    return retIdx;
}