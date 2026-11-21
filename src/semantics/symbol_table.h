#ifndef YAAR_SYMBOL_TABLE_H
#define YAAR_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

enum class SymbolKind {
    VARIABLE,
    ARRAY,
    FUNCTION
};

struct Symbol {
    std::string name;
    std::string type; // "number", "float", "char", "string", "bool", "void"
    SymbolKind kind;
    int scope_level;
    int line_declared;
    int array_size = 0;
    std::vector<std::string> param_types;
    std::string return_type = "";

    Symbol() : kind(SymbolKind::VARIABLE), scope_level(0), line_declared(0) {}
    Symbol(const std::string& name, const std::string& type, SymbolKind kind, int scope, int line)
        : name(name), type(type), kind(kind), scope_level(scope), line_declared(line) {}
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> table;
    SymbolTable* parent_scope;
    int scope_level;

public:
    explicit SymbolTable(SymbolTable* parent = nullptr, int level = 0);
    ~SymbolTable() = default;

    bool insert(const Symbol& sym);
    Symbol* lookup(const std::string& name);
    Symbol* lookupCurrentScope(const std::string& name);
    SymbolTable* getParent() const { return parent_scope; }
    int getScopeLevel() const { return scope_level; }

    void printTable(int indent = 0) const;
};

#endif // YAAR_SYMBOL_TABLE_H
