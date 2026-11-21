#ifndef YAAR_TAC_GENERATOR_H
#define YAAR_TAC_GENERATOR_H

#include "../ast/ast.h"
#include "tac.h"
#include <vector>
#include <string>
#include <stack>

class TACGenerator {
private:
    std::vector<TACInstruction> instructions;
    int temp_counter;
    int label_counter;

    std::stack<std::string> break_labels;
    std::stack<std::string> continue_labels;

    std::string newTemp();
    std::string newLabel(const std::string& prefix = "L");
    void emit(const std::string& op, const std::string& arg1 = "",
              const std::string& arg2 = "", const std::string& result = "");

    std::string genExpr(ExprNode* expr);
    void genStmt(StmtNode* stmt);
    void genVarDecl(VarDeclNode* node);
    void genListDecl(ListDeclNode* node);
    void genAssign(AssignNode* node);
    void genIO(IONode* node);
    void genIf(IfNode* node);
    void genSwitch(SwitchNode* node);
    void genWhile(WhileNode* node);
    void genFor(ForNode* node);
    void genDoWhile(DoWhileNode* node);
    void genFuncDecl(FuncDeclNode* node);
    void genReturn(ReturnNode* node);
    void genBlock(BlockNode* node);

public:
    TACGenerator();
    void generate(ProgramNode* root);
    const std::vector<TACInstruction>& getInstructions() const { return instructions; }
    void printTAC() const;
};

#endif // YAAR_TAC_GENERATOR_H
