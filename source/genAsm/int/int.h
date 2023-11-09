#pragma once

inline void genAsm::genInt(int idx)
{
    //if(prog->sts.at(index).vals.size() > 2 &&
    //    varInScope(&(prog->sts.at(index).vals.at(1).value), scopeStackLoc.size()))
    //{
    //    std::cerr << "Error, variable '" << prog->sts.at(index).vals.at(1).value << "' has already been declared." << std::endl;
    //    exit(1);
    //}
//
    //if(3 < prog->sts.at(index).vals.size() &&
    //    prog->sts.at(index).vals.at(1).type == tokenType::ident &&
    //    prog->sts.at(index).vals.at(2).type == tokenType::equal)
    //{
//
    //    vars.insert({prog->sts.at(index).vals.at(1).value,
    //        var{.stackLoc = stackLoc, .size = 4, .scope = (int)scopeStackLoc.size()}});
//
    //    genExpr(3);
    //    push("edi", SIZE_INT, "DWORD");
//
    //}else{
//
    //    // will be possible in the future tho
    //    std::cerr << "Error, cannot declare variable without a value." << std::endl;
    //    exit(1);
    //}

    if(prog->sts.at(index).vals.size() > idx + 1){
        for(int i = 0; i < prog->sts.at(index).vals.size(); ++i){
            if(prog->sts.at(index).vals.at(i).type == tokenType::ident){
                if(varInScope(&prog->sts.at(index).vals.at(i).value, scopeStackLoc.size())){
                    std::cerr << "Error, identifier '" <<
                        prog->sts.at(index).vals.at(i).value << "' has already been declared in this scope." << std::endl;
                    
                    exit(1);
                }
                
                var v = {.stackLoc = stackLoc, .size = 4, .scope = (int)scopeStackLoc.size()};
                if(prog->sts.at(index).vals.at(i-1).type == tokenType::ptr){
                    v.ptrReadBytes = 4;
                    v.size = 8;
                }

                vars.insert({prog->sts.at(index).vals.at(i).value, v});
                if(i+1 < prog->sts.at(index).vals.size() &&
                    prog->sts.at(index).vals.at(i+1).type == tokenType::equal) // int x = ...;
                {
                    if(i+2 < prog->sts.at(index).vals.size()){
                        genExpr(i+2);
                        push("edi", 4); // dil: low 8 bits of rdi
                    }else{
                        std::cerr << "Error, cannot use assignment operator(=) without a value." << std::endl;
                        exit(1);
                    }
                }else{ // int x;
                    stackLoc += 4; 
                }
            }
        }
    }
}