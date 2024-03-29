#pragma once
#include <sstream>
#include <iostream>
#include <map>
#include <format>
#include "../node/node.h"

//expresion type
struct eType
{
    tokenType type;
    int ptrReadBytes = -1;
};

// the result of generating an expression
struct exprRes
{
    int retIdx;
    tokenType type = (tokenType)0;
};

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
            tokenType type; 
            int ptrReadBytes = -1; // if -1 then not a pointer
            bool isFunction = false;
            bool isExtern = false;
        };

        std::vector<int> scopeStackLoc;

        std::multimap<std::string, var> vars;

        const node::program* prog;

        size_t index = 0;
        size_t stackLoc = 0;
        size_t lableNum = 0;

        inline void addStdLibFunc(const char* funcName); 

        inline void mainInit();
        inline void mainExit();

        // @returns {size_t} lables until closing curly
        size_t findLableCountCurly(size_t stmtIdx);

        // used in genIfExpr for comparing two things
        int cmpTwo(size_t stmtIdx, int idx, tokenType cmpType);

        // used in genIfExpr for comparing two things
        // selects the appropriate jump instruction
        std::string selectJump(tokenType cond, tokenType cmpType);

        // @returns the size to read from pointer. -1 for not found.
        inline int getOprSize(size_t stmtIdx, int idx);

        // @returns {tokenType} the type of the expresion. 0 if not found
        inline eType getType(size_t stmtIdx, int idx);

        // @returns {void*} a var* to the variable in scope
        //(void* because the compiler doesnt like a normal var*)
        void* varInScope(const std::string* varName, int scope);
        
        // @returns{void*} a var* to the variable in the most significant scope
        // or nullptr if there is no such variable
        void* varAccessible(const std::string* varName, int scope);
        
        void push(const char* reg, int size, const char* word = "", const char* mov = "mov");
        void pop(const char* reg, int size, const char* word = "", const char* mov = "mov");

        std::string selectReg(const char* reg, int size);
        std::string selectWord(int size);

        inline std::string handleSpecialChar(const std::string* str);
        inline std::string createTextVarName();
        inline std::string createFloat32VarName();
        size_t textFloat32Count = 0;
        size_t textVarCount = 0;

        void genStmt();

        // the final value is in RAX
        // @returns {exprRes} the index of i. used when there are parenthesis
        exprRes genExpr(size_t stmtIdx, int valsIdx);

        // the result is in RAX
        // @returns {int} the index
        int genMulDiv(int from, size_t stmtIdx, tokenType type);
        
        // @returns {int} the index 
        // @param {const char*} reg
        exprRes genSingle(int idx, const char* reg, size_t stmtIdx, bool checkPostPreIncDec = true, bool ifPtrGetPValue = true);

        int genIfExpr(int from, int lable, size_t stmtIdx, bool invert = false, bool handleCurly = true);

        inline void genExit();
        inline void genInt(int idx = 0);
        inline void genChar(int idx = 0);
        inline void genFloat(int idx = 0);
        inline void genUpdateIdent();
        inline void genCurly(int idx = 0, bool isFromElse = false);
        inline int genPreIncDec(int idx, const char* reg = nullptr);
        inline int genPostIncDec(int idx, const char* reg = nullptr);
        inline void genIf();
        inline int genFunctionCall(int idx);
        inline void genWhile(int idx = 0);
        inline void genFor(int idx = 0);
};