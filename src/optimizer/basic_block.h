#ifndef YAAR_BASIC_BLOCK_H
#define YAAR_BASIC_BLOCK_H

#include "../ir/tac.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct BasicBlock {
    int id;
    std::string name;
    std::vector<TACInstruction> instructions;
    std::vector<BasicBlock*> predecessors;
    std::vector<BasicBlock*> successors;

    explicit BasicBlock(int id = 0, const std::string& name = "")
        : id(id), name(name) {}
};

class ControlFlowGraph {
private:
    std::vector<BasicBlock*> blocks;
    std::unordered_map<std::string, BasicBlock*> label_to_block;

public:
    ControlFlowGraph() = default;
    ~ControlFlowGraph();

    void build(const std::vector<TACInstruction>& instructions);
    const std::vector<BasicBlock*>& getBlocks() const { return blocks; }

    void printBlocks() const;
    std::string toDOT() const;
    void printDOT() const;
};

#endif // YAAR_BASIC_BLOCK_H
