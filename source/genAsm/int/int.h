#pragma once

inline void genAsm::genInt(int idx)
{
    size_t arrSize = 0;
    bool isArr = false;
    if(prog->sts.at(index).vals.size() > idx + 1){
        for(int i = 0; i < prog->sts.at(index).vals.size(); ++i){
            if(prog->sts.at(index).vals.at(i).type == tokenType::bracketOpen){
                if(i+1<prog->sts.at(index).vals.size()){
                    isArr = true; 
                    if(prog->sts.at(index).vals.at(i+1).type == tokenType::intLit){
                        if(i+2 < prog->sts.at(index).vals.size() &&
                            prog->sts.at(index).vals.at(i+2).type == tokenType::bracketClose)
                        {
                            arrSize = std::stoi(prog->sts.at(index).vals.at(i+1).value) * 4;
                        }else{
                            std::cerr << "Error, array declaration with an expresion is currently not supported." << std::endl;
                            exit(1);
                        }
                    }else if(prog->sts.at(index).vals.at(i+1).type == tokenType::bracketClose){ // float[] = ....;
                        // TODO
                    }else{
                        std::cerr << "Error, an array must be declared with a constant expresion. ( int[10] s; )" << std::endl;
                        exit(1);
                    }

                }else{
                    std::cerr << "Error, invalid syntax. (forgot closing bracket?)" << std::endl;
                    exit(1);
                }
            }
            if(prog->sts.at(index).vals.at(i).type == tokenType::ident){
                if(varInScope(&prog->sts.at(index).vals.at(i).value, scopeStackLoc.size())){
                    std::cerr << "Error, identifier '" <<
                        prog->sts.at(index).vals.at(i).value << "' has already been declared in this scope." << std::endl;
                    
                    exit(1);
                }
                
                var v = {.stackLoc = stackLoc, .size = 4, .scope = (int)scopeStackLoc.size(), .type = tokenType::_int};
                if(prog->sts.at(index).vals.at(i-1).type == tokenType::ptr || isArr){
                    v.ptrReadBytes = 4;
                    v.size = 8;
                }
                v.stackLoc += v.size;

                vars.insert({prog->sts.at(index).vals.at(i).value, v});
                if(i+1 < prog->sts.at(index).vals.size() &&
                    prog->sts.at(index).vals.at(i+1).type == tokenType::equal) // int x = ...;
                {
                    if(i+2 < prog->sts.at(index).vals.size()){
                        i = genExpr(index, i+2);
                        push(selectReg("rdi", v.size).c_str(), v.size);
                        i += 2;
                    }else{
                        std::cerr << "Error, cannot use assignment operator(=) without a value." << std::endl;
                        exit(1);
                    }
                }else{ // int x;
                    if(isArr){ // int[10] x;
                        outAsm << "lea rax, [rbp + " << stackLoc + 8 << "]\n\t";
                        push("rax", 8);
                        stackLoc += arrSize;
                    }else{
                        stackLoc += v.size;
                    }
                }
            }
        }
    }
}