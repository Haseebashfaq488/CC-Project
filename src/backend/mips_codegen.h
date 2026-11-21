#ifndef YAAR_MIPS_CODEGEN_H
#define YAAR_MIPS_CODEGEN_H

#include "../ir/tac.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

class MIPSCodeGenerator {
private:
    std::unordered_map<std::string, std::string> string_literals; // string -> label
    std::unordered_set<std::string> declared_vars;
    std::unordered_map<std::string, int> array_sizes;
    int str_counter;

    bool isNumber(const std::string& s) const;
    void collectData(const std::vector<TACInstruction>& instrs);
    std::string loadOperand(const std::string& operand, const std::string& reg);

public:
    MIPSCodeGenerator();
    std::string generate(const std::vector<TACInstruction>& instrs);
    void printMIPS(const std::string& asm_code) const;
};

#endif // YAAR_MIPS_CODEGEN_H
