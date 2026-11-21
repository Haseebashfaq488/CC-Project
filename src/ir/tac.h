#ifndef YAAR_TAC_H
#define YAAR_TAC_H

#include <string>
#include <vector>
#include <iostream>

struct TACInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

    TACInstruction(const std::string& op, const std::string& arg1,
                   const std::string& arg2, const std::string& result)
        : op(op), arg1(arg1), arg2(arg2), result(result) {}

    std::string toString() const {
        if (op == "LABEL") {
            return result + ":";
        }
        if (op == "GOTO") {
            return "    GOTO " + result;
        }
        if (op == "IF_FALSE") {
            return "    IF_FALSE " + arg1 + " GOTO " + result;
        }
        if (op == "IF_TRUE") {
            return "    IF_TRUE " + arg1 + " GOTO " + result;
        }
        if (op == "PARAM") {
            return "    PARAM " + arg1;
        }
        if (op == "CALL") {
            if (!result.empty()) {
                return "    " + result + " = CALL " + arg1 + ", " + arg2;
            }
            return "    CALL " + arg1 + ", " + arg2;
        }
        if (op == "RETURN") {
            if (!arg1.empty()) return "    RETURN " + arg1;
            return "    RETURN";
        }
        if (op == "PRINT") {
            return "    PRINT " + arg1;
        }
        if (op == "READ") {
            return "    READ " + result;
        }
        if (op == "ARRAY_STORE") {
            // arg1 = array, arg2 = index, result = value => array[index] = value
            return "    " + arg1 + "[" + arg2 + "] = " + result;
        }
        if (op == "ARRAY_LOAD") {
            // arg1 = array, arg2 = index, result = temp => temp = array[index]
            return "    " + result + " = " + arg1 + "[" + arg2 + "]";
        }
        if (op == "=" || op == "ASSIGN") {
            return "    " + result + " = " + arg1;
        }
        // Binary Operations
        if (!arg2.empty()) {
            return "    " + result + " = " + arg1 + " " + op + " " + arg2;
        }
        // Unary Operations
        if (!arg1.empty()) {
            return "    " + result + " = " + op + " " + arg1;
        }
        return "    " + op + " " + result;
    }
};

#endif // YAAR_TAC_H
