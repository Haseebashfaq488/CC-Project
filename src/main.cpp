#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "ast/ast.h"
#include "semantics/semantic_analyzer.h"
#include "ir/tac_generator.h"
#include "optimizer/basic_block.h"
#include "optimizer/optimizer.h"
#include "backend/mips_codegen.h"
#include "backend/vm_engine.h"

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
        std::cerr << "  --cfg       Print Basic Blocks & Control Flow Graph (CFG)" << std::endl;
        std::cerr << "  --opt       Run Optimization Passes & print metrics" << std::endl;
        std::cerr << "  --mips      Generate and print MIPS32 Target Assembly" << std::endl;
        std::cerr << "  --run       Execute program interactively via VM Engine" << std::endl;
        std::cerr << "  --all       Run complete pipeline and display all stages" << std::endl;
        return 1;
    }

    bool show_ast = false;
    bool show_symbols = false;
    bool show_tac = false;
    bool show_cfg = false;
    bool show_opt = false;
    bool show_mips = false;
    bool run_vm = false;
    std::string filename = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--ast") show_ast = true;
        else if (arg == "--symbols") show_symbols = true;
        else if (arg == "--tac") show_tac = true;
        else if (arg == "--cfg") show_cfg = true;
        else if (arg == "--opt") show_opt = true;
        else if (arg == "--mips") show_mips = true;
        else if (arg == "--run") run_vm = true;
        else if (arg == "--all") {
            show_ast = true;
            show_symbols = true;
            show_tac = true;
            show_cfg = true;
            show_opt = true;
            show_mips = true;
            run_vm = true;
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

    // AST Display
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

    // Phase 2: TAC Generation
    std::cout << "\n--- Generating Three-Address Code (TAC) ---" << std::endl;
    TACGenerator tac_gen;
    tac_gen.generate(root_ast);
    const auto& raw_tac = tac_gen.getInstructions();

    if (show_tac || (!show_ast && !show_symbols && !show_cfg && !show_opt && !show_mips && !run_vm)) {
        tac_gen.printTAC();
    }
    std::cout << "[SUCCESS] Phase 2.2 Complete: TAC Intermediate Code Generated." << std::endl;

    // Phase 3.1: Control Flow Graph & Basic Blocks
    ControlFlowGraph cfg;
    cfg.build(raw_tac);

    if (show_cfg) {
        cfg.printBlocks();
        cfg.printDOT();
    }

    // Phase 3.2: Optimization Passes
    Optimizer optimizer;
    std::vector<TACInstruction> opt_tac = optimizer.optimize(raw_tac);

    if (show_opt) {
        optimizer.printComparison(raw_tac, opt_tac);
    }

    // Phase 3.3: MIPS32 Target Code Generation
    MIPSCodeGenerator mips_gen;
    std::string mips_asm = mips_gen.generate(opt_tac);

    if (show_mips) {
        mips_gen.printMIPS(mips_asm);
    }

    // Phase 3.4: VM Runtime Execution
    if (run_vm) {
        VMEngine vm;
        vm.execute(opt_tac);
    }

    std::cout << "[SUCCESS] All Compiler Phases Completed Successfully." << std::endl;

    delete root_ast;
    return 0;
}
