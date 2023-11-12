#pragma once
#include <sstream>
#include <iostream>
#include <map>
#include <format>
#include "../node/node.h"

class genAsm
{
    public: 
        genAsm(const node::program* prog, bool lowStdLib);

        std::stringstream finalAsm;
    
    private:

        std::stringstream outAsm;
        std::stringstream secData;
        std::stringstream secText;

        struct var
        {
            size_t stackLoc;
            int size; // in bytes (int = 4, char = 1..)
            int scope;
            int ptrReadBytes = -1; // if -1 then not a pointer
            bool isFunction = false;
            bool isExtern = false;
            const char* stackLocReg = nullptr; 
        };

        std::vector<int> scopeStackLoc;
        std::vector<int> lables;

        std::multimap<std::string, var> vars;

        const node::program* prog;

        size_t index = 0;
        size_t stackLoc = 0;
        size_t lableNum = 0;

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

        inline std::string handleSpecialChar(const std::string* str);
        inline std::string createTextVarName();
        size_t textVarCount = 0;

        void genStmt();

        // the final value is in RDX
        // @returns {int} the index of i. used when there are parenthesis
        int genExpr(int valsIdx = 0);

        // the result is in RAX
        // @returns {int} the index
        int genMulDiv(int from);
        
        // @returns {int} the index 
        // @param {const char*} reg
        int genSingle(int idx, const char* reg);

        int genIfExpr(int from, int lable);

        inline void genExit();
        inline void genInt(int idx = 0);
        inline void genChar(int idx = 0);
        inline void genUpdateIdent();
        inline void genCurly(int idx = 0, bool isFromElse = false);
        inline int genPreIncDec(int idx, const char* reg = nullptr);
        inline int genPostIncDec(int idx, const char* reg = nullptr);
        inline void genIf();
        inline int genFunctionCall(int idx);
};