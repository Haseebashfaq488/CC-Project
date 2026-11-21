%{
#include <iostream>
#include <string>
#include <cstdlib>

extern int yylineno;
extern int yylex();
void yyerror(const char* s);
int syntax_errors = 0;
%}

%union {
    int int_val;
    double float_val;
    char char_val;
    char* str_val;
}

/* Tokens */
%token TOKEN_SUN TOKEN_KHATAM TOKEN_YAAR TOKEN_RAKH
%token TOKEN_TYPE_NUMBER TOKEN_TYPE_FLOAT TOKEN_TYPE_CHAR TOKEN_TYPE_STRING TOKEN_TYPE_BOOL
%token TOKEN_PAKKA TOKEN_TOPI
%token TOKEN_BOL TOKEN_PUCH
%token TOKEN_AGAR TOKEN_WARNA
%token TOKEN_DEKH TOKEN_SARDI TOKEN_GARMI
%token TOKEN_GHOOM TOKEN_JAB TOKEN_TAK TOKEN_CHALA
%token TOKEN_BAS TOKEN_KAR TOKEN_AGAY TOKEN_CHAL
%token TOKEN_JUGAAD TOKEN_DE TOKEN_LIST

%token TOKEN_EQ TOKEN_NEQ TOKEN_LE TOKEN_GE TOKEN_LT TOKEN_GT TOKEN_ASSIGN
%token TOKEN_AND TOKEN_OR TOKEN_NOT
%token TOKEN_PLUS TOKEN_MINUS TOKEN_MULT TOKEN_DIV TOKEN_MOD
%token TOKEN_LPAREN TOKEN_RPAREN TOKEN_LBRACE TOKEN_RBRACE TOKEN_LBRACKET TOKEN_RBRACKET
%token TOKEN_SEMICOLON TOKEN_COMMA TOKEN_COLON

%token <int_val> TOKEN_INT_LITERAL
%token <float_val> TOKEN_FLOAT_LITERAL
%token <char_val> TOKEN_CHAR_LITERAL
%token <str_val> TOKEN_STRING_LITERAL TOKEN_IDENTIFIER

/* Operator Precedence & Associativity */
%nonassoc LOWER_THAN_WARNA
%nonassoc TOKEN_WARNA

%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left TOKEN_LT TOKEN_GT TOKEN_LE TOKEN_GE
%left TOKEN_PLUS TOKEN_MINUS
%left TOKEN_MULT TOKEN_DIV TOKEN_MOD
%right TOKEN_NOT UMINUS

%start program

%%

program:
    TOKEN_SUN TOKEN_YAAR TOKEN_LBRACE statement_list TOKEN_RBRACE TOKEN_KHATAM TOKEN_YAAR
    {
        if (syntax_errors == 0) {
            std::cout << "\n>>> Parsing Successful! YaarLang Program Syntactically Valid. <<<\n" << std::endl;
        } else {
            std::cout << "\n>>> Parsing Completed with " << syntax_errors << " Syntax Error(s). <<<\n" << std::endl;
        }
    }
    ;

statement_list:
    statement_list statement
    | /* empty */
    ;

statement:
    var_decl
    | list_decl
    | assignment_stmt
    | io_stmt
    | if_stmt
    | switch_stmt
    | while_stmt
    | for_stmt
    | dowhile_stmt
    | break_stmt
    | continue_stmt
    | func_decl
    | return_stmt
    | block_stmt
    | error TOKEN_SEMICOLON
      {
          syntax_errors++;
          std::cerr << "Parser Recovery: Resuming parsing after error at line " << yylineno << std::endl;
          yyerrok;
      }
    ;

type:
    TOKEN_TYPE_NUMBER
    | TOKEN_TYPE_FLOAT
    | TOKEN_TYPE_CHAR
    | TOKEN_TYPE_STRING
    | TOKEN_TYPE_BOOL
    ;

var_decl:
    TOKEN_RAKH TOKEN_YAAR type TOKEN_IDENTIFIER TOKEN_ASSIGN expr TOKEN_SEMICOLON
    ;

list_decl:
    TOKEN_LIST TOKEN_YAAR type TOKEN_IDENTIFIER TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET TOKEN_SEMICOLON
    ;

assignment_stmt:
    TOKEN_IDENTIFIER TOKEN_ASSIGN expr TOKEN_SEMICOLON
    | TOKEN_IDENTIFIER TOKEN_LBRACKET expr TOKEN_RBRACKET TOKEN_ASSIGN expr TOKEN_SEMICOLON
    ;

io_stmt:
    TOKEN_BOL TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMICOLON
    | TOKEN_PUCH TOKEN_YAAR TOKEN_LPAREN TOKEN_IDENTIFIER TOKEN_RPAREN TOKEN_SEMICOLON
    ;

if_stmt:
    TOKEN_AGAR TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt %prec LOWER_THAN_WARNA
    | TOKEN_AGAR TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt else_part
    ;

else_part:
    TOKEN_WARNA TOKEN_AGAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt else_part
    | TOKEN_WARNA block_stmt
    ;

switch_stmt:
    TOKEN_DEKH TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_LBRACE case_list TOKEN_RBRACE
    ;

case_list:
    case_list case_clause
    | case_list default_clause
    | /* empty */
    ;

case_clause:
    TOKEN_SARDI literal TOKEN_COLON statement_list
    ;

default_clause:
    TOKEN_GARMI TOKEN_COLON statement_list
    ;

while_stmt:
    TOKEN_JAB TOKEN_TAK TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt
    ;

for_stmt:
    TOKEN_GHOOM TOKEN_YAAR TOKEN_LPAREN for_init TOKEN_SEMICOLON expr TOKEN_SEMICOLON for_step TOKEN_RPAREN block_stmt
    ;

for_init:
    TOKEN_RAKH TOKEN_YAAR type TOKEN_IDENTIFIER TOKEN_ASSIGN expr
    | TOKEN_IDENTIFIER TOKEN_ASSIGN expr
    | /* empty */
    ;

for_step:
    TOKEN_IDENTIFIER TOKEN_ASSIGN expr
    | /* empty */
    ;

dowhile_stmt:
    TOKEN_CHALA TOKEN_YAAR block_stmt TOKEN_JAB TOKEN_TAK TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMICOLON
    ;

break_stmt:
    TOKEN_BAS TOKEN_KAR TOKEN_YAAR TOKEN_SEMICOLON
    ;

continue_stmt:
    TOKEN_AGAY TOKEN_CHAL TOKEN_YAAR TOKEN_SEMICOLON
    ;

func_decl:
    TOKEN_JUGAAD type TOKEN_IDENTIFIER TOKEN_LPAREN param_list TOKEN_RPAREN block_stmt
    ;

param_list:
    param_list_nonempty
    | /* empty */
    ;

param_list_nonempty:
    param_list_nonempty TOKEN_COMMA param
    | param
    ;

param:
    type TOKEN_IDENTIFIER
    ;

return_stmt:
    TOKEN_DE TOKEN_YAAR expr TOKEN_SEMICOLON
    ;

block_stmt:
    TOKEN_LBRACE statement_list TOKEN_RBRACE
    ;

expr:
    expr TOKEN_OR expr
    | expr TOKEN_AND expr
    | expr TOKEN_EQ expr
    | expr TOKEN_NEQ expr
    | expr TOKEN_LT expr
    | expr TOKEN_GT expr
    | expr TOKEN_LE expr
    | expr TOKEN_GE expr
    | expr TOKEN_PLUS expr
    | expr TOKEN_MINUS expr
    | expr TOKEN_MULT expr
    | expr TOKEN_DIV expr
    | expr TOKEN_MOD expr
    | TOKEN_NOT expr
    | TOKEN_MINUS expr %prec UMINUS
    | TOKEN_LPAREN expr TOKEN_RPAREN
    | TOKEN_IDENTIFIER
    | TOKEN_IDENTIFIER TOKEN_LBRACKET expr TOKEN_RBRACKET
    | TOKEN_IDENTIFIER TOKEN_LPAREN arg_list TOKEN_RPAREN
    | literal
    ;

arg_list:
    arg_list_nonempty
    | /* empty */
    ;

arg_list_nonempty:
    arg_list_nonempty TOKEN_COMMA expr
    | expr
    ;

literal:
    TOKEN_INT_LITERAL
    | TOKEN_FLOAT_LITERAL
    | TOKEN_CHAR_LITERAL
    | TOKEN_STRING_LITERAL
    | TOKEN_PAKKA
    | TOKEN_TOPI
    ;

%%

void yyerror(const char* s) {
    syntax_errors++;
    std::cerr << "Syntax Error: " << s << " at line " << yylineno << std::endl;
}
