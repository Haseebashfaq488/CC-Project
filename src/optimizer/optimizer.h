#ifndef YAAR_OPTIMIZER_H
#define YAAR_OPTIMIZER_H

#include "../ir/tac.h"
#include <vector>
#include <string>
#include <unordered_map>

class Optimizer {
private:
    bool passConstantFoldingAndPropagation(std::vector<TACInstruction>& instrs);
    bool passCopyPropagation(std::vector<TACInstruction>& instrs);
    bool passCommonSubexpressionElimination(std::vector<TACInstruction>& instrs);
    bool passDeadCodeElimination(std::vector<TACInstruction>& instrs);

    bool isInteger(const std::string& s) const;
    bool isFloat(const std::string& s) const;
    bool isConstant(const std::string& s) const;

public:
    Optimizer() = default;

    std::vector<TACInstruction> optimize(const std::vector<TACInstruction>& input);
    void printComparison(const std::vector<TACInstruction>& original,
                         const std::vector<TACInstruction>& optimized) const;
};

#endif // YAAR_OPTIMIZER_H
