#ifndef YAAR_AST_H
#define YAAR_AST_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

// Forward declarations
class StmtNode;
class ExprNode;
class CaseNode;

// Base AST Node
class ASTNode {
public:
    int line;
    explicit ASTNode(int line = 0) : line(line) {}
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Expression Base Node
class ExprNode : public ASTNode {
public:
    std::string inferred_type = "unknown";
    explicit ExprNode(int line = 0) : ASTNode(line) {}
};

// Statement Base Node
class StmtNode : public ASTNode {
public:
    explicit StmtNode(int line = 0) : ASTNode(line) {}
};

// Program Node (Root)
class ProgramNode : public ASTNode {
public:
    std::vector<StmtNode*> statements;
    explicit ProgramNode(int line = 0) : ASTNode(line) {}
    ~ProgramNode() override;
    void print(int indent = 0) const override;
};

// Block Node ({ ... })
class BlockNode : public StmtNode {
public:
    std::vector<StmtNode*> statements;
    explicit BlockNode(int line = 0) : StmtNode(line) {}
    ~BlockNode() override;
    void print(int indent = 0) const override;
};

// Variable Declaration Node (rakh yaar <type> <id> = <expr>;)
class VarDeclNode : public StmtNode {
public:
    std::string type;
    std::string name;
    ExprNode* init_expr;
    VarDeclNode(const std::string& type, const std::string& name, ExprNode* init, int line = 0)
        : StmtNode(line), type(type), name(name), init_expr(init) {}
    ~VarDeclNode() override;
    void print(int indent = 0) const override;
};

// Array Declaration Node (list yaar <type> <id>[<size>];)
class ListDeclNode : public StmtNode {
public:
    std::string type;
    std::string name;
    int size;
    ListDeclNode(const std::string& type, const std::string& name, int size, int line = 0)
        : StmtNode(line), type(type), name(name), size(size) {}
    void print(int indent = 0) const override;
};

// Assignment Node (id = expr; or id[idx] = expr;)
class AssignNode : public StmtNode {
public:
    std::string name;
    ExprNode* index_expr; // nullptr if normal scalar assignment
    ExprNode* value_expr;
    AssignNode(const std::string& name, ExprNode* index, ExprNode* value, int line = 0)
        : StmtNode(line), name(name), index_expr(index), value_expr(value) {}
    ~AssignNode() override;
    void print(int indent = 0) const override;
};

// Input/Output Node (bol yaar(expr); or puch yaar(id);)
class IONode : public StmtNode {
public:
    std::string op; // "bol" or "puch"
    ExprNode* expr; // used by bol
    std::string var_name; // used by puch
    IONode(const std::string& op, ExprNode* expr, const std::string& var_name, int line = 0)
        : StmtNode(line), op(op), expr(expr), var_name(var_name) {}
    ~IONode() override;
    void print(int indent = 0) const override;
};

// If-Else Node (agar yaar (...) { } warna agar (...) { } warna { })
class IfNode : public StmtNode {
public:
    ExprNode* condition;
    StmtNode* then_block;
    std::vector<std::pair<ExprNode*, StmtNode*>> elif_branches;
    StmtNode* else_block;
    IfNode(ExprNode* cond, StmtNode* then_b, StmtNode* else_b = nullptr, int line = 0)
        : StmtNode(line), condition(cond), then_block(then_b), else_block(else_b) {}
    ~IfNode() override;
    void print(int indent = 0) const override;
};

// Switch Case Clause Node (sardi <literal>: statements)
class CaseNode : public ASTNode {
public:
    ExprNode* case_literal;
    std::vector<StmtNode*> statements;
    CaseNode(ExprNode* lit, int line = 0) : ASTNode(line), case_literal(lit) {}
    ~CaseNode() override;
    void print(int indent = 0) const override;
};

// Switch Statement Node (dekh yaar (...) { sardi ...: garmi: ... })
class SwitchNode : public StmtNode {
public:
    ExprNode* switch_expr;
    std::vector<CaseNode*> cases;
    std::vector<StmtNode*> default_statements;
    SwitchNode(ExprNode* expr, int line = 0) : StmtNode(line), switch_expr(expr) {}
    ~SwitchNode() override;
    void print(int indent = 0) const override;
};

// While Loop Node (jab tak yaar (...) { ... })
class WhileNode : public StmtNode {
public:
    ExprNode* condition;
    StmtNode* body;
    WhileNode(ExprNode* cond, StmtNode* body, int line = 0)
        : StmtNode(line), condition(cond), body(body) {}
    ~WhileNode() override;
    void print(int indent = 0) const override;
};

// For Loop Node (ghoom yaar (init; cond; step) { ... })
class ForNode : public StmtNode {
public:
    StmtNode* init_stmt;
    ExprNode* condition;
    StmtNode* step_stmt;
    StmtNode* body;
    ForNode(StmtNode* init, ExprNode* cond, StmtNode* step, StmtNode* body, int line = 0)
        : StmtNode(line), init_stmt(init), condition(cond), step_stmt(step), body(body) {}
    ~ForNode() override;
    void print(int indent = 0) const override;
};

// Do-While Loop Node (chala yaar { ... } jab tak yaar (...);)
class DoWhileNode : public StmtNode {
public:
    StmtNode* body;
    ExprNode* condition;
    DoWhileNode(StmtNode* body, ExprNode* cond, int line = 0)
        : StmtNode(line), body(body), condition(cond) {}
    ~DoWhileNode() override;
    void print(int indent = 0) const override;
};

// Break Node (bas kar yaar;)
class BreakNode : public StmtNode {
public:
    explicit BreakNode(int line = 0) : StmtNode(line) {}
    void print(int indent = 0) const override;
};

// Continue Node (agay chal yaar;)
class ContinueNode : public StmtNode {
public:
    explicit ContinueNode(int line = 0) : StmtNode(line) {}
    void print(int indent = 0) const override;
};

// Function Declaration Node (jugaad <type> <id>(params) { ... })
class FuncDeclNode : public StmtNode {
public:
    std::string return_type;
    std::string name;
    std::vector<std::pair<std::string, std::string>> params; // (type, name)
    StmtNode* body;
    FuncDeclNode(const std::string& ret_type, const std::string& name,
                 const std::vector<std::pair<std::string, std::string>>& params,
                 StmtNode* body, int line = 0)
        : StmtNode(line), return_type(ret_type), name(name), params(params), body(body) {}
    ~FuncDeclNode() override;
    void print(int indent = 0) const override;
};

// Return Statement Node (de yaar expr;)
class ReturnNode : public StmtNode {
public:
    ExprNode* expr;
    explicit ReturnNode(ExprNode* expr, int line = 0) : StmtNode(line), expr(expr) {}
    ~ReturnNode() override;
    void print(int indent = 0) const override;
};

// Expression Statement Node (expr;)
class ExprStmtNode : public StmtNode {
public:
    ExprNode* expr;
    explicit ExprStmtNode(ExprNode* expr, int line = 0) : StmtNode(line), expr(expr) {}
    ~ExprStmtNode() override;
    void print(int indent = 0) const override;
};

// Literal Node (numbers, floats, chars, strings, booleans)
class LiteralNode : public ExprNode {
public:
    std::string type_name; // "number", "float", "char", "string", "bool"
    std::string raw_value;
    int int_val = 0;
    double float_val = 0.0;
    char char_val = '\0';
    bool bool_val = false;

    LiteralNode(const std::string& type, const std::string& raw, int line = 0);
    void print(int indent = 0) const override;
};

// Variable Identifier Node
class VariableNode : public ExprNode {
public:
    std::string name;
    VariableNode(const std::string& name, int line = 0) : ExprNode(line), name(name) {}
    void print(int indent = 0) const override;
};

// Array Element Access Node (id[index])
class ArrayAccessNode : public ExprNode {
public:
    std::string name;
    ExprNode* index_expr;
    ArrayAccessNode(const std::string& name, ExprNode* idx, int line = 0)
        : ExprNode(line), name(name), index_expr(idx) {}
    ~ArrayAccessNode() override;
    void print(int indent = 0) const override;
};

// Binary Operation Node (a + b, a == b, etc.)
class BinaryOpNode : public ExprNode {
public:
    std::string op;
    ExprNode* left;
    ExprNode* right;
    BinaryOpNode(const std::string& op, ExprNode* left, ExprNode* right, int line = 0)
        : ExprNode(line), op(op), left(left), right(right) {}
    ~BinaryOpNode() override;
    void print(int indent = 0) const override;
};

// Unary Operation Node (!a, -a)
class UnaryOpNode : public ExprNode {
public:
    std::string op;
    ExprNode* operand;
    UnaryOpNode(const std::string& op, ExprNode* operand, int line = 0)
        : ExprNode(line), op(op), operand(operand) {}
    ~UnaryOpNode() override;
    void print(int indent = 0) const override;
};

// Function Call Node (func(arg1, arg2))
class FuncCallNode : public ExprNode {
public:
    std::string func_name;
    std::vector<ExprNode*> args;
    FuncCallNode(const std::string& name, const std::vector<ExprNode*>& args, int line = 0)
        : ExprNode(line), func_name(name), args(args) {}
    ~FuncCallNode() override;
    void print(int indent = 0) const override;
};

#endif // YAAR_AST_H
