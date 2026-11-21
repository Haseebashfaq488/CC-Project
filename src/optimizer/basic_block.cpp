#include "basic_block.h"
#include <iostream>
#include <sstream>

ControlFlowGraph::~ControlFlowGraph() {
    for (auto b : blocks) delete b;
}

void ControlFlowGraph::build(const std::vector<TACInstruction>& instructions) {
    for (auto b : blocks) delete b;
    blocks.clear();
    label_to_block.clear();

    if (instructions.empty()) return;

    size_t n = instructions.size();
    std::vector<bool> is_leader(n, false);

    // Rule 1: First instruction is a leader
    is_leader[0] = true;

    // Find all branch targets
    std::unordered_set<std::string> branch_targets;
    for (size_t i = 0; i < n; ++i) {
        const auto& instr = instructions[i];
        if (instr.op == "GOTO" || instr.op == "IF_FALSE" || instr.op == "IF_TRUE") {
            branch_targets.insert(instr.result);
            // Rule 3: Instruction immediately following a branch is a leader
            if (i + 1 < n) {
                is_leader[i + 1] = true;
            }
        } else if (instr.op == "RETURN") {
            if (i + 1 < n) {
                is_leader[i + 1] = true;
            }
        }
    }

    // Rule 2: Targets of branches are leaders
    for (size_t i = 0; i < n; ++i) {
        if (instructions[i].op == "LABEL") {
            if (branch_targets.find(instructions[i].result) != branch_targets.end()) {
                is_leader[i] = true;
            }
        }
    }

    // Partition into basic blocks
    int current_id = 0;
    BasicBlock* current_block = nullptr;

    for (size_t i = 0; i < n; ++i) {
        if (is_leader[i]) {
            if (current_block && !current_block->instructions.empty()) {
                blocks.push_back(current_block);
            }
            std::string bname = "B" + std::to_string(current_id);
            current_block = new BasicBlock(current_id++, bname);
        }

        if (!current_block) {
            std::string bname = "B" + std::to_string(current_id);
            current_block = new BasicBlock(current_id++, bname);
        }

        current_block->instructions.push_back(instructions[i]);

        if (instructions[i].op == "LABEL") {
            label_to_block[instructions[i].result] = current_block;
        }
    }

    if (current_block && !current_block->instructions.empty()) {
        blocks.push_back(current_block);
    }

    // Build CFG Edges
    for (size_t i = 0; i < blocks.size(); ++i) {
        BasicBlock* b = blocks[i];
        if (b->instructions.empty()) continue;

        const auto& last = b->instructions.back();

        if (last.op == "GOTO") {
            if (label_to_block.find(last.result) != label_to_block.end()) {
                BasicBlock* target = label_to_block[last.result];
                b->successors.push_back(target);
                target->predecessors.push_back(b);
            }
        } else if (last.op == "IF_FALSE" || last.op == "IF_TRUE") {
            // Branch edge
            if (label_to_block.find(last.result) != label_to_block.end()) {
                BasicBlock* target = label_to_block[last.result];
                b->successors.push_back(target);
                target->predecessors.push_back(b);
            }
            // Fall-through edge
            if (i + 1 < blocks.size()) {
                BasicBlock* next_b = blocks[i + 1];
                b->successors.push_back(next_b);
                next_b->predecessors.push_back(b);
            }
        } else if (last.op == "RETURN") {
            // No successor
        } else {
            // Normal sequential fall-through
            if (i + 1 < blocks.size()) {
                BasicBlock* next_b = blocks[i + 1];
                b->successors.push_back(next_b);
                next_b->predecessors.push_back(b);
            }
        }
    }
}

void ControlFlowGraph::printBlocks() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "      CONTROL FLOW GRAPH (CFG) BLOCKS   " << std::endl;
    std::cout << "========================================" << std::endl;

    for (const auto& b : blocks) {
        std::cout << "\n[" << b->name << "] (Block " << b->id << ")" << std::endl;
        std::cout << "  Predecessors: ";
        if (b->predecessors.empty()) std::cout << "None";
        for (auto p : b->predecessors) std::cout << p->name << " ";
        std::cout << std::endl;

        std::cout << "  Successors:   ";
        if (b->successors.empty()) std::cout << "None (Exit)";
        for (auto s : b->successors) std::cout << s->name << " ";
        std::cout << std::endl;

        std::cout << "  Instructions:" << std::endl;
        for (const auto& instr : b->instructions) {
            std::cout << "    " << instr.toString() << std::endl;
        }
    }
}

std::string ControlFlowGraph::toDOT() const {
    std::ostringstream ss;
    ss << "digraph CFG {\n";
    ss << "  node [shape=box, fontname=\"Courier\"];\n";

    for (const auto& b : blocks) {
        ss << "  " << b->name << " [label=\"" << b->name << "\\n";
        for (const auto& instr : b->instructions) {
            std::string s = instr.toString();
            // escape quotes
            size_t pos = 0;
            while ((pos = s.find("\"", pos)) != std::string::npos) {
                s.replace(pos, 1, "\\\"");
                pos += 2;
            }
            ss << s << "\\n";
        }
        ss << "\"];\n";
    }

    for (const auto& b : blocks) {
        for (auto s : b->successors) {
            ss << "  " << b->name << " -> " << s->name << ";\n";
        }
    }

    ss << "}\n";
    return ss.str();
}

void ControlFlowGraph::printDOT() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "          GRAPHVIZ DOT EXPORT           " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << toDOT() << std::endl;
}
