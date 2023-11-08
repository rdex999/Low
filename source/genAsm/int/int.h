#pragma once

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