#include "symbol_table.h"
#include <iomanip>

SymbolTable::SymbolTable(SymbolTable* parent, int level)
    : parent_scope(parent), scope_level(level) {}

bool SymbolTable::insert(const Symbol& sym) {
    if (table.find(sym.name) != table.end()) {
        return false; // Duplicate symbol in the same scope
    }
    table[sym.name] = sym;
    return true;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    auto it = table.find(name);
    if (it != table.end()) {
        return &it->second;
    }
    if (parent_scope != nullptr) {
        return parent_scope->lookup(name);
    }
    return nullptr;
}

Symbol* SymbolTable::lookupCurrentScope(const std::string& name) {
    auto it = table.find(name);
    if (it != table.end()) {
        return &it->second;
    }
    return nullptr;
}

void SymbolTable::printTable(int indent) const {
    std::string ind(indent * 2, ' ');
    std::cout << ind << "=== Scope Level " << scope_level << " Symbol Table ===" << std::endl;
    std::cout << ind << std::left << std::setw(15) << "Name"
              << std::setw(10) << "Type"
              << std::setw(12) << "Kind"
              << std::setw(8) << "Line"
              << "Details" << std::endl;
    std::cout << ind << std::string(55, '-') << std::endl;

    for (const auto& pair : table) {
        const Symbol& s = pair.second;
        std::string kind_str = (s.kind == SymbolKind::VARIABLE) ? "Variable" :
                               (s.kind == SymbolKind::ARRAY) ? "Array" : "Function";
        std::string details = "";
        if (s.kind == SymbolKind::ARRAY) {
            details = "Size: " + std::to_string(s.array_size);
        } else if (s.kind == SymbolKind::FUNCTION) {
            details = "Params: " + std::to_string(s.param_types.size()) + " -> " + s.return_type;
        }

        std::cout << ind << std::left << std::setw(15) << s.name
                  << std::setw(10) << s.type
                  << std::setw(12) << kind_str
                  << std::setw(8) << s.line_declared
                  << details << std::endl;
    }
}
