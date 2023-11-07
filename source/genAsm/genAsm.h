#pragma once
#include <sstream>
#include <iostream>
#include <map>
#include <format>
#include "../node/node.h"

class genAsm
{
    public: 
        genAsm(const node::program* prog);

        std::stringstream outAsm;
    
    private:

        struct var
        {
            size_t stackLoc;
            int size; // in bytes (int = 4, char = 1..)
            int scope;
        };

        std::vector<int> scopeStackLoc;

        std::multimap<std::string, var> vars;

        const node::program* prog;

        size_t index = 0;
        size_t stackLoc = 0;
        size_t lableCount = 0;

        // @returns {void*} a var* to the variable in scope
        //(void* because the compiler doesnt like a normal var*)
        void* varInScope(const std::string* varName, int scope);
        
        // @returns{void*} a var* to the variable in the most significant scope
        // or nullptr if there is no such variable
        void* varAccessible(const std::string* varName, int scope);
        
        void push(const char* reg, int size, const char* word = "");
        void pop(const char* reg, int size, const char* word = "");

        std::string selectReg(const char* reg, int size);
        std::string selectWord(int size);

        // the final value is in RDX
        // @returns {int} the index of i. used when there are parenthesis
        int genExpr(int valsIdx = 0);

        // the result is in RAX
        // @returns {int} the index
        int genMulDiv(int from);
        
        // @returns {int} the index 
        // @param {const char*} reg
        int genSingle(int idx, const char* reg);

        std::string createLablel(bool peek = false);
        
        int genIfExpr(int from, const std::string* lable);

        inline void genExit();
        inline void genInt();
        inline void genUpdateIdent();
        inline void genCurly(int idx = 0);
        inline void genPreIncDec(int idx, const char* reg = nullptr);
        inline void genPostIncDec(int idx, const char* reg = nullptr);
        inline void genIf();
};