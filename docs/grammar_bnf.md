# YaarLang: Formal BNF Grammar Specification
**Course:** CS 4031 — Compiler Construction  
**Chomsky Hierarchy Classification:** Type 2 (Context-Free Grammar / CFG)  
**Parsing Mechanism:** LALR(1) Bottom-Up Deterministic Shift-Reduce Parser (via GNU Bison)

---

## 1. Chomsky Hierarchy Justification

`YaarLang` is strictly classified as a **Type 2 (Context-Free) Grammar** according to the Chomsky Hierarchy because:
1. Every production is of the form $A \to \alpha$, where $A \in V_N$ (a single non-terminal variable) and $\alpha \in (V_N \cup V_T)^*$ (a finite string of terminals and non-terminals).
2. It requires context-free nested constructs (such as balanced bracket matching `{ }`, nested expressions `( (a + b) * c )`, and recursive function call frames) which cannot be recognized by a Type 3 Regular Grammar (Finite Automata) without memory.
3. It does not contain context-dependent constraints in the syntax layer (e.g., length restrictions $|\alpha| \le |\beta|$ or context anchors $\alpha A \beta \to \alpha \gamma \beta$). Type checking and scope validation are handled systematically in the semantic analysis phase via symbol tables.

---

## 2. Operator Precedence and Associativity

To eliminate ambiguity in arithmetic and logical expressions, the following precedence table is enforced in Bison from lowest to highest precedence:

| Precedence Level | Operators | Associativity | Description |
| :---: | :---: | :---: | :--- |
| 1 (Lowest) | `=` | Right | Assignment |
| 2 | `\|\|` | Left | Logical OR |
| 3 | `&&` | Left | Logical AND |
| 4 | `==`, `!=` | Left | Equality & Inequality |
| 5 | `<`, `<=`, `>`, `>=` | Left | Relational Comparisons |
| 6 | `+`, `-` | Left | Addition & Subtraction |
| 7 | `*`, `/`, `%` | Left | Multiplication, Division, Modulus |
| 8 (Highest) | `!`, Unary `-` | Right | Logical NOT & Negation |

---

## 3. Formal BNF Productions

### 3.1 Program Structure
```bnf
<program>          ::= SUN YAAR LBRACE <statement_list> RBRACE KHATAM YAAR

<statement_list>   ::= <statement> <statement_list>
                     | ε

<statement>        ::= <var_decl>
                     | <assignment_stmt>
                     | <io_stmt>
                     | <if_stmt>
                     | <switch_stmt>
                     | <while_stmt>
                     | <for_stmt>
                     | <dowhile_stmt>
                     | <break_stmt>
                     | <continue_stmt>
                     | <func_decl>
                     | <return_stmt>
                     | <block_stmt>
```

### 3.2 Declarations & Types
```bnf
<type>             ::= NUMBER
                     | FLOAT
                     | CHAR
                     | STRING
                     | BOOL

<var_decl>         ::= RAKH YAAR <type> IDENTIFIER ASSIGN <expr> SEMICOLON
                     | LIST YAAR <type> IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON
```

### 3.3 Assignments & I/O Statements
```bnf
<assignment_stmt>  ::= IDENTIFIER ASSIGN <expr> SEMICOLON
                     | IDENTIFIER LBRACKET <expr> RBRACKET ASSIGN <expr> SEMICOLON

<io_stmt>          ::= BOL YAAR LPAREN <expr> RPAREN SEMICOLON
                     | PUCH YAAR LPAREN IDENTIFIER RPAREN SEMICOLON
```

### 3.4 Control Flow: Conditionals & Switch-Case
```bnf
<if_stmt>          ::= AGAR YAAR LPAREN <expr> RPAREN <block_stmt> <else_part>

<else_part>        ::= WARNA AGAR LPAREN <expr> RPAREN <block_stmt> <else_part>
                     | WARNA <block_stmt>
                     | ε

<switch_stmt>      ::= DEKH YAAR LPAREN <expr> RPAREN LBRACE <case_list> RBRACE

<case_list>        ::= <case_clause> <case_list>
                     | <default_clause>
                     | ε

<case_clause>      ::= SARDI <literal> COLON <statement_list>
<default_clause>   ::= GARMI COLON <statement_list>
```

### 3.5 Loops & Jumps
```bnf
<while_stmt>       ::= JAB TAK YAAR LPAREN <expr> RPAREN <block_stmt>

<for_stmt>         ::= GHOOM YAAR LPAREN <for_init> SEMICOLON <expr> SEMICOLON <for_step> RPAREN <block_stmt>

<for_init>         ::= <var_decl_no_semi>
                     | <assignment_no_semi>
                     | ε

<for_step>         ::= IDENTIFIER ASSIGN <expr>
                     | ε

<dowhile_stmt>     ::= CHALA YAAR <block_stmt> JAB TAK YAAR LPAREN <expr> RPAREN SEMICOLON

<break_stmt>       ::= BAS KAR YAAR SEMICOLON
<continue_stmt>    ::= AGAY CHAL YAAR SEMICOLON
<block_stmt>       ::= LBRACE <statement_list> RBRACE
```

### 3.6 Functions
```bnf
<func_decl>        ::= JUGAAD <type> IDENTIFIER LPAREN <param_list> RPAREN <block_stmt>

<param_list>       ::= <param> COMMA <param_list>
                     | <param>
                     | ε

<param>            ::= <type> IDENTIFIER

<return_stmt>      ::= DE YAAR <expr> SEMICOLON
```

### 3.7 Expressions
```bnf
<expr>             ::= <logical_or_expr>

<logical_or_expr>  ::= <logical_or_expr> OR <logical_and_expr>
                     | <logical_and_expr>

<logical_and_expr> ::= <logical_and_expr> AND <equality_expr>
                     | <equality_expr>

<equality_expr>    ::= <equality_expr> EQ <relational_expr>
                     | <equality_expr> NEQ <relational_expr>
                     | <relational_expr>

<relational_expr>  ::= <relational_expr> LT <additive_expr>
                     | <relational_expr> GT <additive_expr>
                     | <relational_expr> LE <additive_expr>
                     | <relational_expr> GE <additive_expr>
                     | <additive_expr>

<additive_expr>    ::= <additive_expr> PLUS <mult_expr>
                     | <additive_expr> MINUS <mult_expr>
                     | <mult_expr>

<mult_expr>        ::= <mult_expr> MULT <unary_expr>
                     | <mult_expr> DIV <unary_expr>
                     | <mult_expr> MOD <unary_expr>
                     | <unary_expr>

<unary_expr>       ::= NOT <unary_expr>
                     | MINUS <unary_expr>
                     | <primary_expr>

<primary_expr>     ::= IDENTIFIER
                     | IDENTIFIER LBRACKET <expr> RBRACKET
                     | IDENTIFIER LPAREN <arg_list> RPAREN
                     | <literal>
                     | LPAREN <expr> RPAREN

<arg_list>         ::= <expr> COMMA <arg_list>
                     | <expr>
                     | ε

<literal>          ::= INT_LITERAL
                     | FLOAT_LITERAL
                     | CHAR_LITERAL
                     | STRING_LITERAL
                     | PAKKA
                     | TOPI
```
