#include "optimizer.h"
#include <iostream>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <unordered_set>

bool Optimizer::isInteger(const std::string& s) const {
    if (s.empty()) return false;
    size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (start == s.length()) return false;
    for (size_t i = start; i < s.length(); ++i) {
        if (!std::isdigit(s[i])) return false;
    }
    return true;
}

bool Optimizer::isFloat(const std::string& s) const {
    if (s.empty()) return false;
    size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    bool has_dot = false;
    if (start == s.length()) return false;
    for (size_t i = start; i < s.length(); ++i) {
        if (s[i] == '.') {
            if (has_dot) return false;
            has_dot = true;
        } else if (!std::isdigit(s[i])) {
            return false;
        }
    }
    return has_dot;
}

bool Optimizer::isConstant(const std::string& s) const {
    if (s.empty()) return false;
    if (s.front() == '"' && s.back() == '"') return true;
    if (s.front() == '\'' && s.back() == '\'') return true;
    return isInteger(s) || isFloat(s);
}

bool Optimizer::passConstantFoldingAndPropagation(std::vector<TACInstruction>& instrs) {
    bool changed = false;
    std::unordered_map<std::string, std::string> const_map;

    for (auto& instr : instrs) {
        // Clear constant map across jump labels to remain safe across blocks
        if (instr.op == "LABEL" || instr.op == "GOTO" || instr.op == "IF_FALSE" ||
            instr.op == "IF_TRUE" || instr.op == "CALL") {
            const_map.clear();
            continue;
        }

        // Propagate known constants into arg1 and arg2
        if (const_map.find(instr.arg1) != const_map.end()) {
            instr.arg1 = const_map[instr.arg1];
            changed = true;
        }
        if (const_map.find(instr.arg2) != const_map.end()) {
            instr.arg2 = const_map[instr.arg2];
            changed = true;
        }

        // Fold binary integer/float operations
        if (isInteger(instr.arg1) && isInteger(instr.arg2)) {
            long long v1 = std::stoll(instr.arg1);
            long long v2 = std::stoll(instr.arg2);
            long long res = 0;
            bool folded = true;

            if (instr.op == "+") res = v1 + v2;
            else if (instr.op == "-") res = v1 - v2;
            else if (instr.op == "*") res = v1 * v2;
            else if (instr.op == "/" && v2 != 0) res = v1 / v2;
            else if (instr.op == "%" && v2 != 0) res = v1 % v2;
            else if (instr.op == "<") res = (v1 < v2) ? 1 : 0;
            else if (instr.op == "<=") res = (v1 <= v2) ? 1 : 0;
            else if (instr.op == ">") res = (v1 > v2) ? 1 : 0;
            else if (instr.op == ">=") res = (v1 >= v2) ? 1 : 0;
            else if (instr.op == "==") res = (v1 == v2) ? 1 : 0;
            else if (instr.op == "!=") res = (v1 != v2) ? 1 : 0;
            else if (instr.op == "&&") res = (v1 && v2) ? 1 : 0;
            else if (instr.op == "||") res = (v1 || v2) ? 1 : 0;
            else folded = false;

            if (folded) {
                instr.op = "=";
                instr.arg1 = std::to_string(res);
                instr.arg2 = "";
                const_map[instr.result] = instr.arg1;
                changed = true;
                continue;
            }
        }

        // Record simple assignment constants
        if ((instr.op == "=" || instr.op == "ASSIGN") && isConstant(instr.arg1)) {
            const_map[instr.result] = instr.arg1;
        } else {
            // Invalidate if target is reassigned non-constant
            const_map.erase(instr.result);
        }
    }

    return changed;
}

bool Optimizer::passCopyPropagation(std::vector<TACInstruction>& instrs) {
    bool changed = false;
    std::unordered_map<std::string, std::string> copy_map;

    for (auto& instr : instrs) {
        if (instr.op == "LABEL" || instr.op == "GOTO" || instr.op == "IF_FALSE" ||
            instr.op == "IF_TRUE" || instr.op == "CALL") {
            copy_map.clear();
            continue;
        }

        // Replace uses
        if (copy_map.find(instr.arg1) != copy_map.end()) {
            instr.arg1 = copy_map[instr.arg1];
            changed = true;
        }
        if (copy_map.find(instr.arg2) != copy_map.end()) {
            instr.arg2 = copy_map[instr.arg2];
            changed = true;
        }

        // Record copies
        if ((instr.op == "=" || instr.op == "ASSIGN") && !instr.arg1.empty() && instr.arg1[0] != '"' && instr.arg1[0] != '\'') {
            copy_map[instr.result] = instr.arg1;
        } else {
            copy_map.erase(instr.result);
        }
    }

    return changed;
}

bool Optimizer::passCommonSubexpressionElimination(std::vector<TACInstruction>& instrs) {
    bool changed = false;
    // Map expression "op arg1 arg2" -> temporary result variable
    std::unordered_map<std::string, std::string> expr_map;

    for (auto& instr : instrs) {
        if (instr.op == "LABEL" || instr.op == "GOTO" || instr.op == "IF_FALSE" ||
            instr.op == "IF_TRUE" || instr.op == "CALL" || instr.op == "RETURN") {
            expr_map.clear();
            continue;
        }

        // If target was used in an expression, invalidate
        if (!instr.result.empty()) {
            for (auto it = expr_map.begin(); it != expr_map.end();) {
                if (it->first.find(instr.result) != std::string::npos) {
                    it = expr_map.erase(it);
                } else {
                    ++it;
                }
            }
        }

        // Check binary expressions
        if (!instr.arg1.empty() && !instr.arg2.empty() && instr.op != "ARRAY_LOAD" && instr.op != "ARRAY_STORE") {
            std::string expr_key = instr.op + " " + instr.arg1 + " " + instr.arg2;
            if (expr_map.find(expr_key) != expr_map.end()) {
                // Reuse previously computed temporary
                instr.op = "=";
                instr.arg1 = expr_map[expr_key];
                instr.arg2 = "";
                changed = true;
            } else {
                expr_map[expr_key] = instr.result;
            }
        }
    }

    return changed;
}

bool Optimizer::passDeadCodeElimination(std::vector<TACInstruction>& instrs) {
    bool changed = false;
    std::unordered_map<std::string, int> use_count;

    // Count uses of all variables and temporaries
    for (const auto& instr : instrs) {
        if (!instr.arg1.empty() && !isConstant(instr.arg1)) {
            use_count[instr.arg1]++;
        }
        if (!instr.arg2.empty() && !isConstant(instr.arg2)) {
            use_count[instr.arg2]++;
        }
    }

    std::vector<TACInstruction> live_instrs;
    for (const auto& instr : instrs) {
        // Can eliminate temporary variable assignments if unused
        if ((instr.op == "=" || instr.op == "ASSIGN" || instr.op == "+" || instr.op == "-" ||
             instr.op == "*" || instr.op == "/" || instr.op == "%") &&
            !instr.result.empty() && instr.result[0] == 't') {
            if (use_count[instr.result] == 0) {
                // Unused temporary definition eliminated
                changed = true;
                continue;
            }
        }
        live_instrs.push_back(instr);
    }

    if (changed) {
        instrs = live_instrs;
    }
    return changed;
}

std::vector<TACInstruction> Optimizer::optimize(const std::vector<TACInstruction>& input) {
    std::vector<TACInstruction> result = input;
    bool iterating = true;
    int iteration = 0;

    while (iterating && iteration < 10) {
        bool c1 = passConstantFoldingAndPropagation(result);
        bool c2 = passCopyPropagation(result);
        bool c3 = passCommonSubexpressionElimination(result);
        bool c4 = passDeadCodeElimination(result);

        iterating = (c1 || c2 || c3 || c4);
        iteration++;
    }

    return result;
}

void Optimizer::printComparison(const std::vector<TACInstruction>& original,
                                const std::vector<TACInstruction>& optimized) const {
    std::cout << "\n========================================================" << std::endl;
    std::cout << "           CODE OPTIMIZATION METRICS REPORT             " << std::endl;
    std::cout << "========================================================" << std::endl;

    size_t orig_sz = original.size();
    size_t opt_sz = optimized.size();
    double reduction = 0.0;
    if (orig_sz > 0) {
        reduction = ((double)(orig_sz - opt_sz) / (double)orig_sz) * 100.0;
    }

    std::cout << "Original Instructions:  " << orig_sz << std::endl;
    std::cout << "Optimized Instructions: " << opt_sz << std::endl;
    std::cout << "Instructions Removed:   " << (orig_sz - opt_sz) << std::endl;
    std::cout << "Code Size Reduction:    " << std::fixed << std::setprecision(2) << reduction << " %" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << "\n--- Optimized Three-Address Code ---" << std::endl;
    for (const auto& instr : optimized) {
        std::cout << instr.toString() << std::endl;
    }
}
