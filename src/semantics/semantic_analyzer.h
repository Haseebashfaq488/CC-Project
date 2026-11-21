#ifndef YAAR_SEMANTIC_ANALYZER_H
#define YAAR_SEMANTIC_ANALYZER_H

#include "../ast/ast.h"
#include "symbol_table.h"
#include <vector>
#include <string>

class SemanticAnalyzer {
private:
    SymbolTable* current_scope;
    std::vector<SymbolTable*> all_scopes; // for memory cleanup and dump
    std::vector<std::string> errors;
    std::string current_function_return_type;
    int loop_or_switch_depth;

    void logError(int line, const std::string& message);
    void enterScope();
    void exitScope();

    // AST Traversal methods
    void checkStatement(StmtNode* stmt);
    void checkVarDecl(VarDeclNode* node);
    void checkListDecl(ListDeclNode* node);
    void checkAssign(AssignNode* node);
    void checkIO(IONode* node);
    void checkIf(IfNode* node);
    void checkSwitch(SwitchNode* node);
    void checkWhile(WhileNode* node);
    void checkFor(ForNode* node);
    void checkDoWhile(DoWhileNode* node);
    void checkFuncDecl(FuncDeclNode* node);
    void checkReturn(ReturnNode* node);
    void checkBlock(BlockNode* node);

    std::string checkExpr(ExprNode* expr); // Returns inferred type

public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();

    bool analyze(ProgramNode* root);
    const std::vector<std::string>& getErrors() const { return errors; }
    void printAllSymbolTables() const;
};

#endif // YAAR_SEMANTIC_ANALYZER_H
