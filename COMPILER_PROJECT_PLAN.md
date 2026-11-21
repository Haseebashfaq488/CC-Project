# CS 4031: Compiler Construction — Final Semester Project Plan
**Institution:** FAST-NUCES (Faisalabad-Chiniot Campus)  
**Course:** CS 4031 — Compiler Construction  
**Deadline:** `04-12-2026` (Strict & Firm)  
**Evaluation:** **30%** Project Submission & Implementation | **70%** Individual Viva  
**Group Size:** Exactly 3 Members  
**Core Technology:** C / C++ (with Flex & Bison)

---

## 1. Environment & Operating System: Do You Need Ubuntu?

### The Short Answer
**Ubuntu (via WSL on Windows) is strongly recommended.** It is the easiest, industry-standard, and most trouble-free setup for C/C++ compiler development with Flex and Bison.

### Why Ubuntu / WSL?
1. **One-Command Setup**: Installing GNU C/C++, Flex, Bison, Make, GDB, and Valgrind takes 30 seconds on Ubuntu:
   ```bash
   sudo apt update
   sudo apt install -y build-essential flex bison make gdb valgrind
   ```
2. **Native Toolchain Compatibility**: Flex and Bison were designed for POSIX/Linux. On native Windows, ported versions (`win_flex_bison`, MinGW) frequently encounter `<unistd.h>` header issues, CRLF newline problems, and path linking conflicts.
3. **Best of Both Worlds (WSL)**: 
   - You stay on Windows and can use your favorite code editor (VS Code, CLion, Antigravity).
   - Your Windows project folder `C:\CC Project` is instantly accessible inside Ubuntu at `/mnt/c/CC\ Project`.
   - You run builds and tests cleanly inside the Linux terminal.

### Quick Setup Instructions for Ubuntu on Windows (WSL)
1. Open PowerShell as Administrator and run:
   ```powershell
   wsl --install -d Ubuntu
   ```
2. After Ubuntu launches, set up your username and password, then run:
   ```bash
   sudo apt update && sudo apt install -y build-essential flex bison make
   ```
3. Navigate directly to this project folder:
   ```bash
   cd /mnt/c/CC\ Project
   ```

*(Alternative: If you cannot use Ubuntu or WSL, you can install **MSYS2** or **WinFlexBison + MinGW-w64** on Windows, but expect more manual configuration).*

---

## 2. Project Architecture & C/C++ Tech Stack

### Tooling Strategy
- **Lexer (Scanner)**: **Flex** (Fast Lexical Analyzer Generator) or Handcrafted C++ DFA Tokenizer.
- **Parser**: **Bison** (LALR(1) Parser Generator) producing C/C++ AST nodes, or Handcrafted Recursive Descent.
- **Intermediate Representation**: Custom C++ AST Class Hierarchy & Three-Address Code (TAC) Quadruple structures.
- **Optimization Engine**: C++ Control Flow Graph (CFG) analysis and AST/TAC transformations.
- **Target Backend**: Target Assembly generator (e.g., MIPS for MARS/SPIM simulator or x86 assembly) or a lightweight C++ Virtual Machine bytecode emulator.
- **Build System**: `Makefile` or `CMake`.

### Recommended Project Directory Structure
```
c:\CC Project\
│
├── COMPILER_PROJECT_PLAN.md      # This detailed plan
├── Makefile                      # Automated build script (lex, yacc, g++)
│
├── docs/                         # Language specification and viva documentation
│   ├── grammar_bnf.md            # Formal grammar productions & precedence
│   └── language_spec.md          # Language manual, keywords, tokens, semantics
│
├── src/                          # C/C++ Source Code
│   ├── frontend/
│   │   ├── lexer.l               # Flex specification (tokens & regex)
│   │   ├── parser.y              # Bison grammar specification & AST hooks
│   │   └── tokens.h              # Shared token definitions
│   │
│   ├── ast/                      # Abstract Syntax Tree classes
│   │   ├── ast.h                 # AST node base & derived classes
│   │   └── ast.cpp               # AST visualization and printing
│   │
│   ├── semantics/                # Semantic Analysis & Symbol Tables
│   │   ├── symbol_table.h        # Scoped Symbol Table (Global/Function/Block)
│   │   ├── symbol_table.cpp      # Scope management & resolution
│   │   ├── type_checker.h        # Type checking & semantic validation
│   │   └── type_checker.cpp
│   │
│   ├── ir/                       # Intermediate Code Generation (TAC)
│   │   ├── tac.h                 # Three-Address Code data structures (quadruples)
│   │   └── tac_generator.cpp     # AST to TAC translation engine
│   │
│   ├── optimizer/                # Intermediate Code Optimization
│   │   ├── basic_block.h         # CFG and basic block builder
│   │   ├── optimizer.h           # Constant folding, CSE, Dead-code elimination
│   │   └── optimizer.cpp
│   │
│   └── backend/                  # Target Code Generation
│       ├── codegen.h             # Instruction selector & register allocator
│       └── codegen.cpp           # Target Assembly / Bytecode emitter
│
├── tests/                        # Test Programs in Custom Language
│   ├── valid/                    # Test cases for all 11 mandatory features
│   │   ├── 01_arithmetic.txt
│   │   ├── 02_control_flow.txt
│   │   ├── 03_loops.txt
│   │   ├── 04_functions.txt
│   │   ├── 05_arrays.txt
│   │   └── 06_comprehensive.txt  # Fibonacci, Bubble Sort, Factorial
│   └── invalid/                  # Syntax and semantic error test cases
│       ├── syntax_errors.txt
│       └── semantic_errors.txt
│
└── bin/                          # Output binaries
```

---

## 3. Mandatory Language Features Matrix

Every group must design their own syntax, but the following 11 features are **strictly mandatory**:

| Feature Category | Required Functionality |
| :--- | :--- |
| **1. Data Types** | `Integer`, `Float`, `Character`, `String`, `Boolean` |
| **2. Variables** | Declaration, initialization, assignment |
| **3. Arithmetic Operators** | `+`, `-`, `*`, `/`, `%` |
| **4. Relational Operators** | `>`, `<`, `>=`, `<=`, `==`, `!=` |
| **5. Logical Operators** | `AND`, `OR`, `NOT` |
| **6. Conditional Statements**| `IF`, `IF-ELSE`, nested `IF` |
| **7. Selection Statement** | `SWITCH-CASE` with `DEFAULT` |
| **8. Loops** | `FOR`, `WHILE`, `DO-WHILE` |
| **9. Functions** | Declaration, parameters, return statement |
| **10. Arrays** | Declaration, initialization, indexing |
| **11. Input / Output** | Console input (`READ`/`INPUT`) and output (`PRINT`/`DISPLAY`) |

---

## 4. The 3-Phase Execution Plan

```
   ┌─────────────────────────────────────────────────────────────┐
   │ Phase 1: Frontend Foundation                                │
   │ ├── Language Specification & Formal BNF Grammar             │
   │ ├── Flex Lexical Scanner & Input Buffering                  │
   │ └── Bison Syntactic Parser & Syntax Error Recovery          │
   └──────────────────────────────┬──────────────────────────────┘
                                  │
                                  ▼
   ┌─────────────────────────────────────────────────────────────┐
   │ Phase 2: Semantic Analysis & TAC Engine                     │
   │ ├── AST & DAG Construction (C++ Class Hierarchy)            │
   │ ├── Scoped Symbol Table System (Global / Local / Func)      │
   │ ├── Type Checking & Semantic Error Detection                │
   │ └── Three-Address Code (TAC) Quadruple Generator            │
   └──────────────────────────────┬──────────────────────────────┘
                                  │
                                  ▼
   ┌─────────────────────────────────────────────────────────────┐
   │ Phase 3: Optimization, Target Code & Viva Defense           │
   │ ├── Basic Blocks & Control Flow Graph (CFG)                 │
   │ ├── Optimization (Constant Folding, CSE, DCE, Propagation)  │
   │ ├── Target Assembly / Simulator Code Generation             │
   │ └── Full Test Suite & 70% Viva Defense Preparation          │
   └─────────────────────────────────────────────────────────────┘
```

---

### Phase 1: Language Specification & Frontend Foundation

#### Objective
Design the complete programming language, write and resolve its formal grammar, build the token scanner using Flex, and implement the syntactic parser using Bison with error recovery.

#### Prerequisites for Phase 1 (What We Need)
1. **Language Identity Document**:
   - Choose language name and keywords (e.g., `let`, `when`, `otherwise`, `loop`, `show`, `read`).
   - Define exact syntax rules for all 11 mandatory features.
2. **Formal Grammar in BNF**:
   - Context-Free Grammar (CFG) productions.
   - Proof of Chomsky Hierarchy classification (Type 2).
   - Operator precedence & associativity declarations.
   - Elimination of left-recursion, left-factoring, and resolution of dangling-else ambiguity.
3. **Lexer Specification (`lexer.l`)**:
   - Regular expressions for tokens, literals, and comments.
   - Line and column tracking for pinpoint error diagnostics.
4. **Toolchain**:
   - GCC/G++, Flex, Bison, and Make.

#### Detailed Steps for Phase 1
- **Step 1.1**: Author `docs/language_spec.md` with complete syntax definitions and sample scripts.
- **Step 1.2**: Write formal BNF grammar rules in `docs/grammar_bnf.md`; compute FIRST and FOLLOW sets for viva documentation.
- **Step 1.3**: Implement `lexer.l` (Flex scanner) recognizing identifiers, keywords, strings, floats, operators, and handling single/multi-line comments.
- **Step 1.4**: Implement `parser.y` (Bison parser) with explicit `%left`, `%right`, and `%nonassoc` precedence rules.
- **Step 1.5**: Implement syntax error handling using Bison's `error` token for panic-mode error recovery.
- **Step 1.6**: Create `Makefile` to compile Flex + Bison + C++ into an executable frontend binary.

---

### Phase 2: Semantic Analysis, AST & Intermediate Code Generation

#### Objective
Construct the AST, validate type correctness and scope integrity using a hierarchical symbol table, and translate the validated program into linear Three-Address Code (TAC).

#### Detailed Steps for Phase 2
- **Step 2.1**: Design C++ AST classes (`ProgramNode`, `VarDeclNode`, `BinaryOpNode`, `IfNode`, `LoopNode`, `FuncDefNode`, `ArrayAccessNode`).
- **Step 2.2**: Integrate AST node creation into Bison action rules (`$$ = new BinaryOpNode(...)`).
- **Step 2.3**: Build hierarchical Symbol Table (`SymbolTable` class with parent-pointer chaining for nested scopes).
- **Step 2.4**: Implement Semantic Validator & Type Checker:
  - Check for undeclared and redeclared variables.
  - Enforce type compatibility in assignments and binary operations.
  - Validate function argument counts, types, and return statement types.
  - Check array dimensions and integer indexing.
- **Step 2.5**: Design TAC data structure (Quadruples: `struct TAC { string op, arg1, arg2, result; }`).
- **Step 2.6**: Implement TAC generator traversing the AST to produce linear intermediate instructions, temporary variables (`t0, t1, ...`), and branch labels (`L0, L1, ...`).

---

### Phase 3: Intermediate Code Optimization, Target Code & Viva Prep

#### Objective
Optimize TAC to remove redundant computations, generate target machine instructions (e.g., MIPS or x86 assembly), and thoroughly prepare all 3 members for the 70% viva defense.

#### Detailed Steps for Phase 3
- **Step 3.1**: Partition TAC into Basic Blocks by detecting leader instructions.
- **Step 3.2**: Construct the Control Flow Graph (CFG) connecting basic blocks via branch edges.
- **Step 3.3**: Implement Optimization Passes:
  - **Constant Folding & Propagation**: Evaluate constant expressions at compile-time (`2 + 3` $\to$ `5`).
  - **Common Subexpression Elimination (CSE)**: Eliminate duplicate sub-expressions using DAGs.
  - **Dead Code Elimination (DCE)**: Remove instructions assigning to unused temporaries.
  - **Copy Propagation**: Replace temporary copies with their original operands.
- **Step 3.4**: Build Target Code Generator:
  - Map TAC operations to target instructions (e.g., MIPS `lw`, `sw`, `add`, `sub`, `beq`, `j`, `syscall`).
  - Implement register allocation (allocating available registers `$t0-$t9`, `$s0-$s7` or spilling to stack).
- **Step 3.5**: Full Test Suite Verification:
  - Run benchmark programs (Factorial, Bubble Sort, Fibonacci, Matrix Addition).
  - Print side-by-side comparison of original TAC vs. optimized TAC with percentage reduction metrics.
- **Step 3.6**: Viva Defense Rehearsal:
  - Prepare answers to all standard compiler viva questions (Grammar ambiguities, shift/reduce conflicts, LALR vs SLR, symbol table chaining, register allocation strategies).

---

## 5. Workload Distribution (3 Team Members)

Because **70% of the course grade depends on the individual viva**, every team member must have clear primary ownership while maintaining full understanding of all other components:

| Member | Primary Modules | Secondary / Defense Responsibilities |
| :--- | :--- | :--- |
| **Member 1 (Frontend Lead)** | Language design, BNF grammar, Flex Scanner (`lexer.l`), Bison Parser (`parser.y`), syntax error handling | FIRST/FOLLOW sets, parsing tables (LL/LR), grammar conflict resolution |
| **Member 2 (Middle-End Lead)**| AST / DAG hierarchy, Scoped Symbol Tables, Semantic Analyzer & Type Checker | Attribute grammars (SDD/SDT), scope resolution, type coercion rules |
| **Member 3 (Backend Lead)** | TAC generator, Basic Blocks & CFG, Code Optimizer passes, Target Code Generator | Register allocation algorithms, instruction selection, optimization proof |

---

## 6. Recommended Next Step: Ready to Start Phase 1?

When you are ready, we will immediately start **Phase 1**:
1. Name your programming language.
2. Decide your syntax style (C-style with `{}` or Pascal/Python style with keywords).
3. Generate the formal `language_spec.md` and initial `lexer.l` / `parser.y` files.
