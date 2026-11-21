#include "vm_engine.h"
#include <iostream>
#include <cctype>

std::string VMValue::toString() const {
    if (type == NUMBER) return std::to_string(int_val);
    if (type == FLOAT) return std::to_string(float_val);
    if (type == STRING) return str_val;
    if (type == BOOL) return bool_val ? "pakka" : "topi";
    if (type == CHAR) return std::string(1, char_val);
    return "nil";
}

bool VMValue::isTrue() const {
    if (type == BOOL) return bool_val;
    if (type == NUMBER) return int_val != 0;
    if (type == FLOAT) return float_val != 0.0;
    return false;
}

bool VMEngine::isLiteral(const std::string& s) const {
    if (s.empty()) return false;
    if (s.front() == '"' && s.back() == '"') return true;
    if (s.front() == '\'' && s.back() == '\'') return true;
    size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (start == s.length()) return false;
    bool has_digit = false;
    bool has_dot = false;
    for (size_t i = start; i < s.length(); ++i) {
        if (s[i] == '.') {
            if (has_dot) return false;
            has_dot = true;
        } else if (std::isdigit(s[i])) {
            has_digit = true;
        } else {
            return false;
        }
    }
    return has_digit;
}

VMValue VMEngine::resolve(const std::string& operand) {
    if (operand.empty()) return VMValue();

    // String literal
    if (operand.front() == '"' && operand.back() == '"') {
        return VMValue(operand.substr(1, operand.length() - 2));
    }
    // Char literal
    if (operand.front() == '\'' && operand.back() == '\'') {
        return VMValue(operand[1]);
    }
    // Number / Float literal
    if (isLiteral(operand)) {
        if (operand.find('.') != std::string::npos) {
            return VMValue(std::stod(operand));
        }
        return VMValue(std::stoll(operand));
    }

    // Lookup variable in memory
    auto it = memory.find(operand);
    if (it != memory.end()) {
        return it->second;
    }
    return VMValue(0LL); // Default 0
}

void VMEngine::execute(const std::vector<TACInstruction>& instrs) {
    memory.clear();
    arrays.clear();
    label_to_pc.clear();
    while (!call_stack.empty()) call_stack.pop();
    param_buffer.clear();

    std::cout << "\n========================================================" << std::endl;
    std::cout << "         YAARLANG RUNTIME VM EXECUTION OUTPUT           " << std::endl;
    std::cout << "========================================================" << std::endl;

    // First pass: Index all labels
    for (size_t i = 0; i < instrs.size(); ++i) {
        if (instrs[i].op == "LABEL") {
            label_to_pc[instrs[i].result] = i;
        }
    }

    size_t pc = 0;
    while (pc < instrs.size()) {
        const auto& instr = instrs[pc];

        if (instr.op == "LABEL") {
            pc++;
            continue;
        }

        if (instr.op == "=" || instr.op == "ASSIGN") {
            memory[instr.result] = resolve(instr.arg1);
            pc++;
            continue;
        }

        if (instr.op == "+" || instr.op == "-" || instr.op == "*" ||
            instr.op == "/" || instr.op == "%") {
            VMValue v1 = resolve(instr.arg1);
            VMValue v2 = resolve(instr.arg2);

            if (v1.type == VMValue::STRING && v2.type == VMValue::STRING && instr.op == "+") {
                memory[instr.result] = VMValue(v1.str_val + v2.str_val);
            } else if (v1.type == VMValue::FLOAT || v2.type == VMValue::FLOAT) {
                double n1 = (v1.type == VMValue::FLOAT) ? v1.float_val : (double)v1.int_val;
                double n2 = (v2.type == VMValue::FLOAT) ? v2.float_val : (double)v2.int_val;
                double res = 0.0;
                if (instr.op == "+") res = n1 + n2;
                else if (instr.op == "-") res = n1 - n2;
                else if (instr.op == "*") res = n1 * n2;
                else if (instr.op == "/" && n2 != 0.0) res = n1 / n2;
                memory[instr.result] = VMValue(res);
            } else {
                long long n1 = v1.int_val;
                long long n2 = v2.int_val;
                long long res = 0;
                if (instr.op == "+") res = n1 + n2;
                else if (instr.op == "-") res = n1 - n2;
                else if (instr.op == "*") res = n1 * n2;
                else if (instr.op == "/" && n2 != 0) res = n1 / n2;
                else if (instr.op == "%" && n2 != 0) res = n1 % n2;
                memory[instr.result] = VMValue(res);
            }
            pc++;
            continue;
        }

        if (instr.op == "==" || instr.op == "!=" || instr.op == "<" ||
            instr.op == "<=" || instr.op == ">" || instr.op == ">=") {
            VMValue v1 = resolve(instr.arg1);
            VMValue v2 = resolve(instr.arg2);
            bool res = false;

            if (v1.type == VMValue::STRING && v2.type == VMValue::STRING) {
                if (instr.op == "==") res = (v1.str_val == v2.str_val);
                else if (instr.op == "!=") res = (v1.str_val != v2.str_val);
            } else {
                double n1 = (v1.type == VMValue::FLOAT) ? v1.float_val : (double)v1.int_val;
                double n2 = (v2.type == VMValue::FLOAT) ? v2.float_val : (double)v2.int_val;
                if (instr.op == "==") res = (n1 == n2);
                else if (instr.op == "!=") res = (n1 != n2);
                else if (instr.op == "<") res = (n1 < n2);
                else if (instr.op == "<=") res = (n1 <= n2);
                else if (instr.op == ">") res = (n1 > n2);
                else if (instr.op == ">=") res = (n1 >= n2);
            }
            memory[instr.result] = VMValue(res);
            pc++;
            continue;
        }

        if (instr.op == "&&" || instr.op == "||") {
            bool b1 = resolve(instr.arg1).isTrue();
            bool b2 = resolve(instr.arg2).isTrue();
            bool res = (instr.op == "&&") ? (b1 && b2) : (b1 || b2);
            memory[instr.result] = VMValue(res);
            pc++;
            continue;
        }

        if (instr.op == "!") {
            bool b = resolve(instr.arg1).isTrue();
            memory[instr.result] = VMValue(!b);
            pc++;
            continue;
        }

        if (instr.op == "GOTO") {
            pc = label_to_pc[instr.result];
            continue;
        }

        if (instr.op == "IF_FALSE") {
            if (!resolve(instr.arg1).isTrue()) {
                pc = label_to_pc[instr.result];
            } else {
                pc++;
            }
            continue;
        }

        if (instr.op == "IF_TRUE") {
            if (resolve(instr.arg1).isTrue()) {
                pc = label_to_pc[instr.result];
            } else {
                pc++;
            }
            continue;
        }

        if (instr.op == "PRINT") {
            std::cout << resolve(instr.arg1).toString() << std::endl;
            pc++;
            continue;
        }

        if (instr.op == "READ") {
            std::cout << "Input > ";
            std::string input_str;
            if (std::cin >> input_str) {
                if (isLiteral(input_str)) {
                    if (input_str.find('.') != std::string::npos) {
                        memory[instr.result] = VMValue(std::stod(input_str));
                    } else {
                        memory[instr.result] = VMValue(std::stoll(input_str));
                    }
                } else {
                    memory[instr.result] = VMValue(input_str);
                }
            }
            pc++;
            continue;
        }

        if (instr.op == "ALLOC_ARRAY") {
            int sz = std::stoi(instr.arg1);
            arrays[instr.result] = std::vector<VMValue>(sz, VMValue(0LL));
            pc++;
            continue;
        }

        if (instr.op == "ARRAY_STORE") {
            size_t idx = (size_t)resolve(instr.arg2).int_val;
            if (arrays.find(instr.arg1) != arrays.end() && idx < arrays[instr.arg1].size()) {
                arrays[instr.arg1][idx] = resolve(instr.result);
            }
            pc++;
            continue;
        }

        if (instr.op == "ARRAY_LOAD") {
            size_t idx = (size_t)resolve(instr.arg2).int_val;
            if (arrays.find(instr.arg1) != arrays.end() && idx < arrays[instr.arg1].size()) {
                memory[instr.result] = arrays[instr.arg1][idx];
            } else {
                memory[instr.result] = VMValue(0LL);
            }
            pc++;
            continue;
        }

        if (instr.op == "PARAM") {
            param_buffer.push_back(resolve(instr.arg1));
            pc++;
            continue;
        }

        if (instr.op == "CALL") {
            call_stack.push(pc + 1);
            pc = label_to_pc[instr.arg1];
            continue;
        }

        if (instr.op == "RETURN") {
            if (!instr.arg1.empty()) {
                return_register = resolve(instr.arg1);
            }
            if (call_stack.empty()) {
                break; // main return / exit
            }
            size_t return_pc = call_stack.top();
            call_stack.pop();
            // Assign return value to call target
            if (return_pc > 0 && !instrs[return_pc - 1].result.empty()) {
                memory[instrs[return_pc - 1].result] = return_register;
            }
            pc = return_pc;
            continue;
        }

        pc++;
    }

    std::cout << "========================================================\n" << std::endl;
}
