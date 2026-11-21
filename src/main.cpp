#include <iostream>
#include <cstdio>
#include <cstdlib>

extern FILE* yyin;
extern int yyparse();
extern int syntax_errors;

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  YaarLang Compiler Frontend (v1.0.0)   " << std::endl;
    std::cout << "  Course: CS 4031 Compiler Construction " << std::endl;
    std::cout << "========================================" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.yaar>" << std::endl;
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error: Could not open source file '" << argv[1] << "'." << std::endl;
        return 1;
    }

    yyin = file;
    std::cout << "Compiling: " << argv[1] << " ..." << std::endl;

    int parse_result = yyparse();
    fclose(file);

    if (parse_result == 0 && syntax_errors == 0) {
        std::cout << "[SUCCESS] Frontend Parsing Complete: Zero Errors Detected." << std::endl;
        return 0;
    } else {
        std::cerr << "[FAILURE] Frontend Parsing Finished with Errors." << std::endl;
        return 1;
    }
}
