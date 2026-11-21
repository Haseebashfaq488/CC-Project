#ifndef YAAR_VM_ENGINE_H
#define YAAR_VM_ENGINE_H

#include "../ir/tac.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <stack>

struct VMValue {
    enum Type { NUMBER, FLOAT, STRING, BOOL, CHAR, NIL } type;
    long long int_val = 0;
    double float_val = 0.0;
    std::string str_val = "";
    bool bool_val = false;
    char char_val = '\0';

    VMValue() : type(NIL) {}
    explicit VMValue(long long v) : type(NUMBER), int_val(v) {}
    explicit VMValue(double v) : type(FLOAT), float_val(v) {}
    explicit VMValue(const std::string& v) : type(STRING), str_val(v) {}
    explicit VMValue(bool v) : type(BOOL), bool_val(v) {}
    explicit VMValue(char v) : type(CHAR), char_val(v) {}

    std::string toString() const;
    bool isTrue() const;
};

class VMEngine {
private:
    std::unordered_map<std::string, VMValue> memory;
    std::unordered_map<std::string, std::vector<VMValue>> arrays;
    std::unordered_map<std::string, size_t> label_to_pc;
    std::stack<size_t> call_stack;
    std::vector<VMValue> param_buffer;
    VMValue return_register;

    VMValue resolve(const std::string& operand);
    bool isLiteral(const std::string& s) const;

public:
    VMEngine() = default;
    void execute(const std::vector<TACInstruction>& instrs);
};

#endif // YAAR_VM_ENGINE_H
