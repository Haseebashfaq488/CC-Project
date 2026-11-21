#include "semantic_analyzer.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer()
    : current_scope(nullptr), current_function_return_type(""), loop_or_switch_depth(0) {
    current_scope = new SymbolTable(nullptr, 0);
    all_scopes.push_back(current_scope);
}

SemanticAnalyzer::~SemanticAnalyzer() {
    for (auto st : all_scopes) {
        delete st;
    }
}

void SemanticAnalyzer::logError(int line, const std::string& message) {
    std::string err = "Semantic Error (Line " + std::to_string(line) + "): " + message;
    errors.push_back(err);
}

void SemanticAnalyzer::enterScope() {
    int next_level = current_scope ? current_scope->getScopeLevel() + 1 : 0;
    current_scope = new SymbolTable(current_scope, next_level);
    all_scopes.push_back(current_scope);
}

void SemanticAnalyzer::exitScope() {
    if (current_scope && current_scope->getParent()) {
        current_scope = current_scope->getParent();
    }
}

void SemanticAnalyzer::printAllSymbolTables() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "         SYMBOL TABLE DUMPS             " << std::endl;
    std::cout << "========================================" << std::endl;
    for (size_t i = 0; i < all_scopes.size(); ++i) {
        all_scopes[i]->printTable();
        std::cout << std::endl;
    }
}

bool SemanticAnalyzer::analyze(ProgramNode* root) {
    if (!root) return false;

    for (auto stmt : root->statements) {
        checkStatement(stmt);
    }

    if (!errors.empty()) {
        std::cerr << "\n>>> Semantic Analysis Failed with " << errors.size() << " Error(s): <<<" << std::endl;
        for (const auto& err : errors) {
            std::cerr << "  " << err << std::endl;
        }
        return false;
    }

    std::cout << ">>> Semantic Analysis Successful: Zero Errors Detected. <<<" << std::endl;
    return true;
}

void SemanticAnalyzer::checkStatement(StmtNode* stmt) {
    if (!stmt) return;

    if (auto node = dynamic_cast<VarDeclNode*>(stmt)) {
        checkVarDecl(node);
    } else if (auto node = dynamic_cast<ListDeclNode*>(stmt)) {
        checkListDecl(node);
    } else if (auto node = dynamic_cast<AssignNode*>(stmt)) {
        checkAssign(node);
    } else if (auto node = dynamic_cast<IONode*>(stmt)) {
        checkIO(node);
    } else if (auto node = dynamic_cast<IfNode*>(stmt)) {
        checkIf(node);
    } else if (auto node = dynamic_cast<SwitchNode*>(stmt)) {
        checkSwitch(node);
    } else if (auto node = dynamic_cast<WhileNode*>(stmt)) {
        checkWhile(node);
    } else if (auto node = dynamic_cast<ForNode*>(stmt)) {
        checkFor(node);
    } else if (auto node = dynamic_cast<DoWhileNode*>(stmt)) {
        checkDoWhile(node);
    } else if (auto node = dynamic_cast<FuncDeclNode*>(stmt)) {
        checkFuncDecl(node);
    } else if (auto node = dynamic_cast<ReturnNode*>(stmt)) {
        checkReturn(node);
    } else if (auto node = dynamic_cast<BlockNode*>(stmt)) {
        checkBlock(node);
    } else if (dynamic_cast<BreakNode*>(stmt)) {
        if (loop_or_switch_depth <= 0) {
            logError(stmt->line, "'bas kar yaar' (break) used outside loop or switch.");
        }
    } else if (dynamic_cast<ContinueNode*>(stmt)) {
        if (loop_or_switch_depth <= 0) {
            logError(stmt->line, "'agay chal yaar' (continue) used outside loop.");
        }
    } else if (auto node = dynamic_cast<ExprStmtNode*>(stmt)) {
        checkExpr(node->expr);
    }
}

void SemanticAnalyzer::checkVarDecl(VarDeclNode* node) {
    if (current_scope->lookupCurrentScope(node->name)) {
        logError(node->line, "Redeclaration of identifier '" + node->name + "' in the same scope.");
        return;
    }

    std::string init_type = checkExpr(node->init_expr);
    if (!init_type.empty() && init_type != "unknown") {
        if (node->type != init_type) {
            // Allow implicit promotion: number to float
            if (!(node->type == "float" && init_type == "number")) {
                logError(node->line, "Type mismatch in declaration of '" + node->name +
                                     "'. Declared as '" + node->type + "' but initialized with '" + init_type + "'.");
            }
        }
    }

    Symbol sym(node->name, node->type, SymbolKind::VARIABLE,
               current_scope->getScopeLevel(), node->line);
    current_scope->insert(sym);
}

void SemanticAnalyzer::checkListDecl(ListDeclNode* node) {
    if (current_scope->lookupCurrentScope(node->name)) {
        logError(node->line, "Redeclaration of array '" + node->name + "' in the same scope.");
        return;
    }

    if (node->size <= 0) {
        logError(node->line, "Array size for '" + node->name + "' must be greater than 0.");
    }

    Symbol sym(node->name, node->type, SymbolKind::ARRAY,
               current_scope->getScopeLevel(), node->line);
    sym.array_size = node->size;
    current_scope->insert(sym);
}

void SemanticAnalyzer::checkAssign(AssignNode* node) {
    Symbol* sym = current_scope->lookup(node->name);
    if (!sym) {
        logError(node->line, "Assignment to undeclared identifier '" + node->name + "'.");
        return;
    }

    if (node->index_expr) {
        if (sym->kind != SymbolKind::ARRAY) {
            logError(node->line, "Subscript applied to non-array variable '" + node->name + "'.");
        }
        std::string idx_type = checkExpr(node->index_expr);
        if (idx_type != "number") {
            logError(node->line, "Array index for '" + node->name + "' must evaluate to 'number', got '" + idx_type + "'.");
        }
    } else {
        if (sym->kind == SymbolKind::ARRAY) {
            logError(node->line, "Cannot assign scalar value directly to array '" + node->name + "'.");
        }
    }

    std::string val_type = checkExpr(node->value_expr);
    if (!val_type.empty() && val_type != "unknown") {
        if (sym->type != val_type) {
            if (!(sym->type == "float" && val_type == "number")) {
                logError(node->line, "Type mismatch in assignment to '" + node->name +
                                     "'. Variable is '" + sym->type + "', expression is '" + val_type + "'.");
            }
        }
    }
}

void SemanticAnalyzer::checkIO(IONode* node) {
    if (node->op == "bol") {
        checkExpr(node->expr);
    } else if (node->op == "puch") {
        Symbol* sym = current_scope->lookup(node->var_name);
        if (!sym) {
            logError(node->line, "puch yaar() targets undeclared identifier '" + node->var_name + "'.");
        }
    }
}

void SemanticAnalyzer::checkIf(IfNode* node) {
    std::string cond_type = checkExpr(node->condition);
    if (cond_type != "bool" && cond_type != "unknown") {
        logError(node->line, "Condition in 'agar yaar' must evaluate to 'bool', got '" + cond_type + "'.");
    }

    checkStatement(node->then_block);

    for (const auto& branch : node->elif_branches) {
        std::string elif_cond = checkExpr(branch.first);
        if (elif_cond != "bool" && elif_cond != "unknown") {
            logError(branch.first->line, "Condition in 'warna agar' must evaluate to 'bool', got '" + elif_cond + "'.");
        }
        checkStatement(branch.second);
    }

    if (node->else_block) {
        checkStatement(node->else_block);
    }
}

void SemanticAnalyzer::checkSwitch(SwitchNode* node) {
    std::string expr_type = checkExpr(node->switch_expr);
    loop_or_switch_depth++;

    for (auto c : node->cases) {
        std::string case_type = checkExpr(c->case_literal);
        if (case_type != expr_type && expr_type != "unknown") {
            logError(c->line, "Case literal type '" + case_type +
                              "' does not match switch expression type '" + expr_type + "'.");
        }
        for (auto stmt : c->statements) {
            checkStatement(stmt);
        }
    }

    for (auto s : node->default_statements) {
        checkStatement(s);
    }

    loop_or_switch_depth--;
}

void SemanticAnalyzer::checkWhile(WhileNode* node) {
    std::string cond_type = checkExpr(node->condition);
    if (cond_type != "bool" && cond_type != "unknown") {
        logError(node->line, "Condition in 'jab tak yaar' must evaluate to 'bool', got '" + cond_type + "'.");
    }

    loop_or_switch_depth++;
    checkStatement(node->body);
    loop_or_switch_depth--;
}

void SemanticAnalyzer::checkFor(ForNode* node) {
    enterScope();
    if (node->init_stmt) checkStatement(node->init_stmt);

    if (node->condition) {
        std::string cond_type = checkExpr(node->condition);
        if (cond_type != "bool" && cond_type != "unknown") {
            logError(node->line, "Condition in 'ghoom yaar' must evaluate to 'bool', got '" + cond_type + "'.");
        }
    }

    if (node->step_stmt) checkStatement(node->step_stmt);

    loop_or_switch_depth++;
    checkStatement(node->body);
    loop_or_switch_depth--;

    exitScope();
}

void SemanticAnalyzer::checkDoWhile(DoWhileNode* node) {
    loop_or_switch_depth++;
    checkStatement(node->body);
    loop_or_switch_depth--;

    std::string cond_type = checkExpr(node->condition);
    if (cond_type != "bool" && cond_type != "unknown") {
        logError(node->line, "Condition in 'chala yaar ... jab tak yaar' must evaluate to 'bool', got '" + cond_type + "'.");
    }
}

void SemanticAnalyzer::checkFuncDecl(FuncDeclNode* node) {
    if (current_scope->lookupCurrentScope(node->name)) {
        logError(node->line, "Redeclaration of function '" + node->name + "'.");
        return;
    }

    Symbol sym(node->name, node->return_type, SymbolKind::FUNCTION,
               current_scope->getScopeLevel(), node->line);
    sym.return_type = node->return_type;
    for (const auto& param : node->params) {
        sym.param_types.push_back(param.first);
    }
    current_scope->insert(sym);

    enterScope();
    std::string prev_ret = current_function_return_type;
    current_function_return_type = node->return_type;

    for (const auto& param : node->params) {
        Symbol param_sym(param.second, param.first, SymbolKind::VARIABLE,
                         current_scope->getScopeLevel(), node->line);
        if (!current_scope->insert(param_sym)) {
            logError(node->line, "Duplicate parameter name '" + param.second + "' in function '" + node->name + "'.");
        }
    }

    checkStatement(node->body);

    current_function_return_type = prev_ret;
    exitScope();
}

void SemanticAnalyzer::checkReturn(ReturnNode* node) {
    if (current_function_return_type.empty()) {
        logError(node->line, "'de yaar' (return) used outside of any function.");
        return;
    }

    std::string ret_type = checkExpr(node->expr);
    if (ret_type != current_function_return_type) {
        if (!(current_function_return_type == "float" && ret_type == "number")) {
            logError(node->line, "Function return type mismatch. Expected '" + current_function_return_type +
                                 "', got '" + ret_type + "'.");
        }
    }
}

void SemanticAnalyzer::checkBlock(BlockNode* node) {
    enterScope();
    for (auto stmt : node->statements) {
        checkStatement(stmt);
    }
    exitScope();
}

std::string SemanticAnalyzer::checkExpr(ExprNode* expr) {
    if (!expr) return "unknown";

    if (auto lit = dynamic_cast<LiteralNode*>(expr)) {
        expr->inferred_type = lit->type_name;
        return lit->type_name;
    }

    if (auto var = dynamic_cast<VariableNode*>(expr)) {
        Symbol* sym = current_scope->lookup(var->name);
        if (!sym) {
            logError(var->line, "Undeclared identifier '" + var->name + "'.");
            expr->inferred_type = "unknown";
            return "unknown";
        }
        expr->inferred_type = sym->type;
        return sym->type;
    }

    if (auto arr = dynamic_cast<ArrayAccessNode*>(expr)) {
        Symbol* sym = current_scope->lookup(arr->name);
        if (!sym) {
            logError(arr->line, "Undeclared array identifier '" + arr->name + "'.");
            return "unknown";
        }
        if (sym->kind != SymbolKind::ARRAY) {
            logError(arr->line, "Identifier '" + arr->name + "' is not an array.");
        }
        std::string idx_type = checkExpr(arr->index_expr);
        if (idx_type != "number") {
            logError(arr->line, "Array subscript for '" + arr->name + "' must be 'number', got '" + idx_type + "'.");
        }
        expr->inferred_type = sym->type;
        return sym->type;
    }

    if (auto bin = dynamic_cast<BinaryOpNode*>(expr)) {
        std::string left_t = checkExpr(bin->left);
        std::string right_t = checkExpr(bin->right);

        if (bin->op == "+" || bin->op == "-" || bin->op == "*" || bin->op == "/") {
            if (bin->op == "+" && left_t == "string" && right_t == "string") {
                bin->inferred_type = "string";
                return "string";
            }
            if ((left_t == "number" || left_t == "float") &&
                (right_t == "number" || right_t == "float")) {
                std::string res = (left_t == "float" || right_t == "float") ? "float" : "number";
                bin->inferred_type = res;
                return res;
            }
            logError(bin->line, "Invalid operand types for '" + bin->op + "': '" + left_t + "' and '" + right_t + "'.");
            return "unknown";
        }

        if (bin->op == "%") {
            if (left_t == "number" && right_t == "number") {
                bin->inferred_type = "number";
                return "number";
            }
            logError(bin->line, "Modulus operator '%' requires integer operands, got '" + left_t + "' and '" + right_t + "'.");
            return "unknown";
        }

        if (bin->op == "<" || bin->op == "<=" || bin->op == ">" || bin->op == ">=") {
            if ((left_t == "number" || left_t == "float") &&
                (right_t == "number" || right_t == "float")) {
                bin->inferred_type = "bool";
                return "bool";
            }
            logError(bin->line, "Comparison operator '" + bin->op + "' requires numeric operands.");
            return "bool";
        }

        if (bin->op == "==" || bin->op == "!=") {
            if (left_t != right_t && !( (left_t == "number" && right_t == "float") || (left_t == "float" && right_t == "number") )) {
                logError(bin->line, "Equality operator '" + bin->op + "' compares incompatible types '" + left_t + "' and '" + right_t + "'.");
            }
            bin->inferred_type = "bool";
            return "bool";
        }

        if (bin->op == "&&" || bin->op == "||") {
            if (left_t != "bool" || right_t != "bool") {
                logError(bin->line, "Logical operator '" + bin->op + "' requires boolean operands, got '" + left_t + "' and '" + right_t + "'.");
            }
            bin->inferred_type = "bool";
            return "bool";
        }
    }

    if (auto un = dynamic_cast<UnaryOpNode*>(expr)) {
        std::string op_t = checkExpr(un->operand);
        if (un->op == "!") {
            if (op_t != "bool") {
                logError(un->line, "Logical NOT '!' requires boolean operand, got '" + op_t + "'.");
            }
            un->inferred_type = "bool";
            return "bool";
        }
        if (un->op == "-") {
            if (op_t != "number" && op_t != "float") {
                logError(un->line, "Unary '-' requires numeric operand, got '" + op_t + "'.");
            }
            un->inferred_type = op_t;
            return op_t;
        }
    }

    if (auto call = dynamic_cast<FuncCallNode*>(expr)) {
        Symbol* sym = current_scope->lookup(call->func_name);
        if (!sym) {
            logError(call->line, "Call to undeclared function '" + call->func_name + "'.");
            return "unknown";
        }
        if (sym->kind != SymbolKind::FUNCTION) {
            logError(call->line, "'" + call->func_name + "' is not a function.");
            return "unknown";
        }

        if (call->args.size() != sym->param_types.size()) {
            logError(call->line, "Function '" + call->func_name + "' expects " +
                                 std::to_string(sym->param_types.size()) + " argument(s), got " +
                                 std::to_string(call->args.size()) + ".");
        } else {
            for (size_t i = 0; i < call->args.size(); ++i) {
                std::string arg_t = checkExpr(call->args[i]);
                if (arg_t != sym->param_types[i]) {
                    logError(call->line, "Argument " + std::to_string(i + 1) + " of function '" + call->func_name +
                                         "' expects type '" + sym->param_types[i] + "', got '" + arg_t + "'.");
                }
            }
        }

        call->inferred_type = sym->return_type;
        return sym->return_type;
    }

    return "unknown";
}
