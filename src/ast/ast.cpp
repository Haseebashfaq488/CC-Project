#include "ast.h"
#include <iomanip>

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}

// ProgramNode
ProgramNode::~ProgramNode() {
    for (auto stmt : statements) delete stmt;
}

void ProgramNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ProgramNode (sun yaar ... khatam yaar)" << std::endl;
    for (const auto& stmt : statements) {
        if (stmt) stmt->print(indent + 1);
    }
}

// BlockNode
BlockNode::~BlockNode() {
    for (auto stmt : statements) delete stmt;
}

void BlockNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BlockNode {" << std::endl;
    for (const auto& stmt : statements) {
        if (stmt) stmt->print(indent + 1);
    }
    printIndent(indent);
    std::cout << "}" << std::endl;
}

// VarDeclNode
VarDeclNode::~VarDeclNode() {
    delete init_expr;
}

void VarDeclNode::print(int indent) const {
    printIndent(indent);
    std::cout << "VarDecl: " << type << " " << name << std::endl;
    if (init_expr) {
        printIndent(indent + 1);
        std::cout << "InitExpr:" << std::endl;
        init_expr->print(indent + 2);
    }
}

// ListDeclNode
void ListDeclNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ListDecl: " << type << " " << name << "[" << size << "]" << std::endl;
}

// AssignNode
AssignNode::~AssignNode() {
    delete index_expr;
    delete value_expr;
}

void AssignNode::print(int indent) const {
    printIndent(indent);
    std::cout << "AssignNode: " << name;
    if (index_expr) {
        std::cout << "[";
        index_expr->print(0);
        std::cout << "]";
    }
    std::cout << " =" << std::endl;
    if (value_expr) value_expr->print(indent + 1);
}

// IONode
IONode::~IONode() {
    delete expr;
}

void IONode::print(int indent) const {
    printIndent(indent);
    if (op == "bol") {
        std::cout << "IONode: bol yaar (Print)" << std::endl;
        if (expr) expr->print(indent + 1);
    } else {
        std::cout << "IONode: puch yaar (Read into " << var_name << ")" << std::endl;
    }
}

// IfNode
IfNode::~IfNode() {
    delete condition;
    delete then_block;
    for (auto& branch : elif_branches) {
        delete branch.first;
        delete branch.second;
    }
    delete else_block;
}

void IfNode::print(int indent) const {
    printIndent(indent);
    std::cout << "IfNode (agar yaar):" << std::endl;
    printIndent(indent + 1);
    std::cout << "Condition:" << std::endl;
    if (condition) condition->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Then:" << std::endl;
    if (then_block) then_block->print(indent + 2);

    for (const auto& branch : elif_branches) {
        printIndent(indent);
        std::cout << "Warna Agar:" << std::endl;
        if (branch.first) branch.first->print(indent + 2);
        if (branch.second) branch.second->print(indent + 2);
    }

    if (else_block) {
        printIndent(indent);
        std::cout << "Warna (Else):" << std::endl;
        else_block->print(indent + 1);
    }
}

// CaseNode
CaseNode::~CaseNode() {
    delete case_literal;
    for (auto s : statements) delete s;
}

void CaseNode::print(int indent) const {
    printIndent(indent);
    std::cout << "CaseNode (sardi):" << std::endl;
    if (case_literal) case_literal->print(indent + 1);
    for (const auto& s : statements) {
        if (s) s->print(indent + 1);
    }
}

// SwitchNode
SwitchNode::~SwitchNode() {
    delete switch_expr;
    for (auto c : cases) delete c;
    for (auto s : default_statements) delete s;
}

void SwitchNode::print(int indent) const {
    printIndent(indent);
    std::cout << "SwitchNode (dekh yaar):" << std::endl;
    if (switch_expr) switch_expr->print(indent + 1);
    for (const auto& c : cases) {
        if (c) c->print(indent + 1);
    }
    if (!default_statements.empty()) {
        printIndent(indent);
        std::cout << "Default (garmi):" << std::endl;
        for (const auto& s : default_statements) {
            if (s) s->print(indent + 1);
        }
    }
}

// WhileNode
WhileNode::~WhileNode() {
    delete condition;
    delete body;
}

void WhileNode::print(int indent) const {
    printIndent(indent);
    std::cout << "WhileNode (jab tak yaar):" << std::endl;
    printIndent(indent + 1);
    std::cout << "Condition:" << std::endl;
    if (condition) condition->print(indent + 2);
    printIndent(indent + 1);
    std::cout << "Body:" << std::endl;
    if (body) body->print(indent + 2);
}

// ForNode
ForNode::~ForNode() {
    delete init_stmt;
    delete condition;
    delete step_stmt;
    delete body;
}

void ForNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ForNode (ghoom yaar):" << std::endl;
    if (init_stmt) {
        printIndent(indent + 1);
        std::cout << "Init:" << std::endl;
        init_stmt->print(indent + 2);
    }
    if (condition) {
        printIndent(indent + 1);
        std::cout << "Condition:" << std::endl;
        condition->print(indent + 2);
    }
    if (step_stmt) {
        printIndent(indent + 1);
        std::cout << "Step:" << std::endl;
        step_stmt->print(indent + 2);
    }
    if (body) {
        printIndent(indent + 1);
        std::cout << "Body:" << std::endl;
        body->print(indent + 2);
    }
}

// DoWhileNode
DoWhileNode::~DoWhileNode() {
    delete body;
    delete condition;
}

void DoWhileNode::print(int indent) const {
    printIndent(indent);
    std::cout << "DoWhileNode (chala yaar ... jab tak yaar):" << std::endl;
    if (body) body->print(indent + 1);
    printIndent(indent);
    std::cout << "Condition:" << std::endl;
    if (condition) condition->print(indent + 1);
}

// Break & Continue
void BreakNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BreakNode (bas kar yaar)" << std::endl;
}

void ContinueNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ContinueNode (agay chal yaar)" << std::endl;
}

// FuncDeclNode
FuncDeclNode::~FuncDeclNode() {
    delete body;
}

void FuncDeclNode::print(int indent) const {
    printIndent(indent);
    std::cout << "FuncDecl: " << return_type << " " << name << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        std::cout << params[i].first << " " << params[i].second;
        if (i + 1 < params.size()) std::cout << ", ";
    }
    std::cout << ")" << std::endl;
    if (body) body->print(indent + 1);
}

// ReturnNode
ReturnNode::~ReturnNode() {
    delete expr;
}

void ReturnNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ReturnNode (de yaar):" << std::endl;
    if (expr) expr->print(indent + 1);
}

// ExprStmtNode
ExprStmtNode::~ExprStmtNode() {
    delete expr;
}

void ExprStmtNode::print(int indent) const {
    if (expr) expr->print(indent);
}

// LiteralNode
LiteralNode::LiteralNode(const std::string& type, const std::string& raw, int line)
    : ExprNode(line), type_name(type), raw_value(raw) {
    inferred_type = type;
    if (type == "number") {
        int_val = std::stoi(raw);
    } else if (type == "float") {
        float_val = std::stod(raw);
    } else if (type == "char") {
        char_val = raw.empty() ? '\0' : raw[0];
    } else if (type == "bool") {
        bool_val = (raw == "pakka");
    }
}

void LiteralNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Literal (" << type_name << "): " << raw_value << std::endl;
}

// VariableNode
void VariableNode::print(int indent) const {
    printIndent(indent);
    std::cout << "Variable: " << name;
    if (!inferred_type.empty() && inferred_type != "unknown") {
        std::cout << " [type: " << inferred_type << "]";
    }
    std::cout << std::endl;
}

// ArrayAccessNode
ArrayAccessNode::~ArrayAccessNode() {
    delete index_expr;
}

void ArrayAccessNode::print(int indent) const {
    printIndent(indent);
    std::cout << "ArrayAccess: " << name << "[" << std::endl;
    if (index_expr) index_expr->print(indent + 1);
    printIndent(indent);
    std::cout << "]" << std::endl;
}

// BinaryOpNode
BinaryOpNode::~BinaryOpNode() {
    delete left;
    delete right;
}

void BinaryOpNode::print(int indent) const {
    printIndent(indent);
    std::cout << "BinaryOp: " << op;
    if (!inferred_type.empty() && inferred_type != "unknown") {
        std::cout << " [type: " << inferred_type << "]";
    }
    std::cout << std::endl;
    if (left) left->print(indent + 1);
    if (right) right->print(indent + 1);
}

// UnaryOpNode
UnaryOpNode::~UnaryOpNode() {
    delete operand;
}

void UnaryOpNode::print(int indent) const {
    printIndent(indent);
    std::cout << "UnaryOp: " << op << std::endl;
    if (operand) operand->print(indent + 1);
}

// FuncCallNode
FuncCallNode::~FuncCallNode() {
    for (auto arg : args) delete arg;
}

void FuncCallNode::print(int indent) const {
    printIndent(indent);
    std::cout << "FuncCall: " << func_name << "()" << std::endl;
    for (const auto& arg : args) {
        if (arg) arg->print(indent + 1);
    }
}
