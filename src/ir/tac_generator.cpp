#include "tac_generator.h"
#include <iostream>

TACGenerator::TACGenerator() : temp_counter(0), label_counter(0) {}

std::string TACGenerator::newTemp() {
    return "t" + std::to_string(temp_counter++);
}

std::string TACGenerator::newLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(label_counter++);
}

void TACGenerator::emit(const std::string& op, const std::string& arg1,
                        const std::string& arg2, const std::string& result) {
    instructions.emplace_back(op, arg1, arg2, result);
}

void TACGenerator::printTAC() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "      THREE-ADDRESS CODE (TAC / IR)     " << std::endl;
    std::cout << "========================================" << std::endl;
    for (const auto& instr : instructions) {
        std::cout << instr.toString() << std::endl;
    }
}

void TACGenerator::generate(ProgramNode* root) {
    if (!root) return;
    emit("LABEL", "", "", "main");
    for (auto stmt : root->statements) {
        genStmt(stmt);
    }
    emit("LABEL", "", "", "program_end");
}

void TACGenerator::genStmt(StmtNode* stmt) {
    if (!stmt) return;

    if (auto node = dynamic_cast<VarDeclNode*>(stmt)) {
        genVarDecl(node);
    } else if (auto node = dynamic_cast<ListDeclNode*>(stmt)) {
        genListDecl(node);
    } else if (auto node = dynamic_cast<AssignNode*>(stmt)) {
        genAssign(node);
    } else if (auto node = dynamic_cast<IONode*>(stmt)) {
        genIO(node);
    } else if (auto node = dynamic_cast<IfNode*>(stmt)) {
        genIf(node);
    } else if (auto node = dynamic_cast<SwitchNode*>(stmt)) {
        genSwitch(node);
    } else if (auto node = dynamic_cast<WhileNode*>(stmt)) {
        genWhile(node);
    } else if (auto node = dynamic_cast<ForNode*>(stmt)) {
        genFor(node);
    } else if (auto node = dynamic_cast<DoWhileNode*>(stmt)) {
        genDoWhile(node);
    } else if (auto node = dynamic_cast<FuncDeclNode*>(stmt)) {
        genFuncDecl(node);
    } else if (auto node = dynamic_cast<ReturnNode*>(stmt)) {
        genReturn(node);
    } else if (auto node = dynamic_cast<BlockNode*>(stmt)) {
        genBlock(node);
    } else if (dynamic_cast<BreakNode*>(stmt)) {
        if (!break_labels.empty()) {
            emit("GOTO", "", "", break_labels.top());
        }
    } else if (dynamic_cast<ContinueNode*>(stmt)) {
        if (!continue_labels.empty()) {
            emit("GOTO", "", "", continue_labels.top());
        }
    } else if (auto node = dynamic_cast<ExprStmtNode*>(stmt)) {
        genExpr(node->expr);
    }
}

void TACGenerator::genVarDecl(VarDeclNode* node) {
    if (node->init_expr) {
        std::string val = genExpr(node->init_expr);
        emit("=", val, "", node->name);
    }
}

void TACGenerator::genListDecl(ListDeclNode* node) {
    emit("ALLOC_ARRAY", std::to_string(node->size), "", node->name);
}

void TACGenerator::genAssign(AssignNode* node) {
    std::string val = genExpr(node->value_expr);
    if (node->index_expr) {
        std::string idx = genExpr(node->index_expr);
        emit("ARRAY_STORE", node->name, idx, val);
    } else {
        emit("=", val, "", node->name);
    }
}

void TACGenerator::genIO(IONode* node) {
    if (node->op == "bol") {
        std::string val = genExpr(node->expr);
        emit("PRINT", val, "", "");
    } else if (node->op == "puch") {
        emit("READ", "", "", node->var_name);
    }
}

void TACGenerator::genIf(IfNode* node) {
    std::string cond = genExpr(node->condition);
    std::string else_label = newLabel("L_else");
    std::string end_label = newLabel("L_endif");

    emit("IF_FALSE", cond, "", else_label);
    genStmt(node->then_block);
    emit("GOTO", "", "", end_label);

    emit("LABEL", "", "", else_label);

    for (const auto& branch : node->elif_branches) {
        std::string next_branch = newLabel("L_elif");
        std::string branch_cond = genExpr(branch.first);
        emit("IF_FALSE", branch_cond, "", next_branch);
        genStmt(branch.second);
        emit("GOTO", "", "", end_label);
        emit("LABEL", "", "", next_branch);
    }

    if (node->else_block) {
        genStmt(node->else_block);
    }

    emit("LABEL", "", "", end_label);
}

void TACGenerator::genSwitch(SwitchNode* node) {
    std::string switch_val = genExpr(node->switch_expr);
    std::string end_switch = newLabel("L_endswitch");
    break_labels.push(end_switch);

    std::vector<std::pair<std::string, CaseNode*>> case_labels;
    for (auto c : node->cases) {
        std::string clabel = newLabel("L_case");
        case_labels.push_back({clabel, c});
    }

    std::string default_label = newLabel("L_default");

    // Dispatch table / comparisons
    for (const auto& pair : case_labels) {
        std::string lit = genExpr(pair.second->case_literal);
        std::string cmp = newTemp();
        emit("==", switch_val, lit, cmp);
        emit("IF_TRUE", cmp, "", pair.first);
    }

    emit("GOTO", "", "", default_label);

    // Case bodies
    for (const auto& pair : case_labels) {
        emit("LABEL", "", "", pair.first);
        for (auto s : pair.second->statements) {
            genStmt(s);
        }
    }

    // Default body
    emit("LABEL", "", "", default_label);
    for (auto s : node->default_statements) {
        genStmt(s);
    }

    emit("LABEL", "", "", end_switch);
    break_labels.pop();
}

void TACGenerator::genWhile(WhileNode* node) {
    std::string start_label = newLabel("L_while_start");
    std::string end_label = newLabel("L_while_end");

    continue_labels.push(start_label);
    break_labels.push(end_label);

    emit("LABEL", "", "", start_label);
    std::string cond = genExpr(node->condition);
    emit("IF_FALSE", cond, "", end_label);

    genStmt(node->body);

    emit("GOTO", "", "", start_label);
    emit("LABEL", "", "", end_label);

    continue_labels.pop();
    break_labels.pop();
}

void TACGenerator::genFor(ForNode* node) {
    if (node->init_stmt) genStmt(node->init_stmt);

    std::string start_label = newLabel("L_for_start");
    std::string step_label = newLabel("L_for_step");
    std::string end_label = newLabel("L_for_end");

    continue_labels.push(step_label);
    break_labels.push(end_label);

    emit("LABEL", "", "", start_label);

    if (node->condition) {
        std::string cond = genExpr(node->condition);
        emit("IF_FALSE", cond, "", end_label);
    }

    genStmt(node->body);

    emit("LABEL", "", "", step_label);
    if (node->step_stmt) genStmt(node->step_stmt);

    emit("GOTO", "", "", start_label);
    emit("LABEL", "", "", end_label);

    continue_labels.pop();
    break_labels.pop();
}

void TACGenerator::genDoWhile(DoWhileNode* node) {
    std::string start_label = newLabel("L_dowhile_start");
    std::string cond_label = newLabel("L_dowhile_cond");
    std::string end_label = newLabel("L_dowhile_end");

    continue_labels.push(cond_label);
    break_labels.push(end_label);

    emit("LABEL", "", "", start_label);
    genStmt(node->body);

    emit("LABEL", "", "", cond_label);
    std::string cond = genExpr(node->condition);
    emit("IF_TRUE", cond, "", start_label);

    emit("LABEL", "", "", end_label);

    continue_labels.pop();
    break_labels.pop();
}

void TACGenerator::genFuncDecl(FuncDeclNode* node) {
    std::string func_end = newLabel("L_func_end_" + node->name);
    emit("GOTO", "", "", func_end); // Skip function body in normal linear execution

    emit("LABEL", "", "", "func_" + node->name);
    genStmt(node->body);
    emit("RETURN", "", "", ""); // Safety return

    emit("LABEL", "", "", func_end);
}

void TACGenerator::genReturn(ReturnNode* node) {
    std::string ret_val = "";
    if (node->expr) {
        ret_val = genExpr(node->expr);
    }
    emit("RETURN", ret_val, "", "");
}

void TACGenerator::genBlock(BlockNode* node) {
    for (auto stmt : node->statements) {
        genStmt(stmt);
    }
}

std::string TACGenerator::genExpr(ExprNode* expr) {
    if (!expr) return "";

    if (auto lit = dynamic_cast<LiteralNode*>(expr)) {
        if (lit->type_name == "string") {
            return "\"" + lit->raw_value + "\"";
        }
        if (lit->type_name == "char") {
            return "'" + lit->raw_value + "'";
        }
        if (lit->type_name == "bool") {
            return (lit->raw_value == "pakka") ? "1" : "0";
        }
        return lit->raw_value;
    }

    if (auto var = dynamic_cast<VariableNode*>(expr)) {
        return var->name;
    }

    if (auto arr = dynamic_cast<ArrayAccessNode*>(expr)) {
        std::string idx = genExpr(arr->index_expr);
        std::string t = newTemp();
        emit("ARRAY_LOAD", arr->name, idx, t);
        return t;
    }

    if (auto bin = dynamic_cast<BinaryOpNode*>(expr)) {
        std::string left = genExpr(bin->left);
        std::string right = genExpr(bin->right);
        std::string t = newTemp();
        emit(bin->op, left, right, t);
        return t;
    }

    if (auto un = dynamic_cast<UnaryOpNode*>(expr)) {
        std::string opnd = genExpr(un->operand);
        std::string t = newTemp();
        emit(un->op, opnd, "", t);
        return t;
    }

    if (auto call = dynamic_cast<FuncCallNode*>(expr)) {
        std::vector<std::string> arg_temps;
        for (auto arg : call->args) {
            arg_temps.push_back(genExpr(arg));
        }
        for (const auto& a : arg_temps) {
            emit("PARAM", a, "", "");
        }
        std::string t = newTemp();
        emit("CALL", "func_" + call->func_name, std::to_string(arg_temps.size()), t);
        return t;
    }

    return "";
}
