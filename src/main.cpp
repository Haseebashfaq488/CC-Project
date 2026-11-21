#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "ast/ast.h"
#include "semantics/semantic_analyzer.h"
#include "ir/tac_generator.h"

extern FILE* yyin;
extern int yyparse();
extern int syntax_errors;
extern ProgramNode* root_ast;

void printBanner() {
    std::cout << "========================================" << std::endl;
    std::cout << "        YaarLang Compiler (yaarc)       " << std::endl;
    std::cout << "  Course: CS 4031 Compiler Construction " << std::endl;
    std::cout << "========================================" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printBanner();
        std::cerr << "Usage: " << argv[0] << " [options] <source_file.yaar>" << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  --ast       Print the Abstract Syntax Tree (AST)" << std::endl;
        std::cerr << "  --symbols   Print the Scoped Symbol Table Dumps" << std::endl;
        std::cerr << "  --tac       Print the Three-Address Code (TAC / IR)" << std::endl;
        std::cerr << "  --all       Run complete pipeline and print all stages" << std::endl;
        return 1;
    }

    bool show_ast = false;
    bool show_symbols = false;
    bool show_tac = false;
    std::string filename = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--ast") {
            show_ast = true;
        } else if (arg == "--symbols") {
            show_symbols = true;
        } else if (arg == "--tac") {
            show_tac = true;
        } else if (arg == "--all") {
            show_ast = true;
            show_symbols = true;
            show_tac = true;
        } else if (arg[0] != '-') {
            filename = arg;
        }
    }

    if (filename.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        return 1;
    }

    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        std::cerr << "Error: Could not open source file '" << filename << "'." << std::endl;
        return 1;
    }

    printBanner();
    std::cout << "Compiling: " << filename << " ...\n" << std::endl;

    yyin = file;
    int parse_result = yyparse();
    fclose(file);

    if (parse_result != 0 || syntax_errors > 0 || root_ast == nullptr) {
        std::cerr << "\n[FAILURE] Compilation Aborted: Syntax Errors Detected." << std::endl;
        delete root_ast;
        return 1;
    }

    std::cout << "[SUCCESS] Phase 1 Complete: 0 Syntax Errors Detected." << std::endl;

    // Optional: Print AST
    if (show_ast) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "       ABSTRACT SYNTAX TREE (AST)       " << std::endl;
        std::cout << "========================================" << std::endl;
        root_ast->print(0);
    }

    // Phase 2: Semantic Analysis & Symbol Tables
    std::cout << "\n--- Starting Semantic Analysis & Type Checking ---" << std::endl;
    SemanticAnalyzer analyzer;
    bool semantic_success = analyzer.analyze(root_ast);

    if (show_symbols) {
        analyzer.printAllSymbolTables();
    }

    if (!semantic_success) {
        std::cerr << "\n[FAILURE] Compilation Aborted: Semantic Verification Failed." << std::endl;
        delete root_ast;
        return 1;
    }

    std::cout << "[SUCCESS] Phase 2.1 Complete: Semantic Analysis & Type Safety Verified." << std::endl;

    // Phase 2: Three-Address Code (TAC) Generation
    std::cout << "\n--- Generating Three-Address Code (TAC) ---" << std::endl;
    TACGenerator tac_gen;
    tac_gen.generate(root_ast);

    if (show_tac || (!show_ast && !show_symbols)) {
        tac_gen.printTAC();
    }

    std::cout << "\n[SUCCESS] Phase 2.2 Complete: TAC Intermediate Code Generated." << std::endl;

    delete root_ast;
    return 0;
}
