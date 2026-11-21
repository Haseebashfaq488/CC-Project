%code requires {
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <cstring>
#include "../ast/ast.h"
}

%{
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "../ast/ast.h"

extern int yylineno;
extern int yylex();
void yyerror(const char* s);

int syntax_errors = 0;
ProgramNode* root_ast = nullptr;
%}

%union {
    int int_val;
    double float_val;
    char char_val;
    char* str_val;

    ASTNode* ast_node;
    ProgramNode* program_node;
    StmtNode* stmt_node;
    ExprNode* expr_node;
    CaseNode* case_node;

    std::vector<StmtNode*>* stmt_list;
    std::vector<CaseNode*>* case_list;
    std::vector<ExprNode*>* arg_list;
    std::vector<std::pair<std::string, std::string>>* param_list;
    std::vector<std::pair<ExprNode*, StmtNode*>>* elif_list;
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

/* Non-terminal Types */
%type <program_node> program
%type <stmt_list> statement_list default_clause
%type <stmt_node> statement var_decl list_decl assignment_stmt io_stmt
%type <stmt_node> if_stmt switch_stmt while_stmt for_stmt dowhile_stmt
%type <stmt_node> break_stmt continue_stmt func_decl return_stmt block_stmt
%type <stmt_node> for_init for_step

%type <str_val> type
%type <expr_node> expr literal
%type <case_list> case_list
%type <case_node> case_clause
%type <elif_list> elif_list
%type <param_list> param_list param_list_nonempty
%type <arg_list> arg_list arg_list_nonempty

/* Precedence */
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
        $$ = new ProgramNode(yylineno);
        if ($4) {
            $$->statements = *$4;
            delete $4;
        }
        root_ast = $$;
    }
    ;

statement_list:
    statement_list statement
    {
        $$ = $1;
        if ($2) $$->push_back($2);
    }
    | /* empty */
    {
        $$ = new std::vector<StmtNode*>();
    }
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
          $$ = nullptr;
      }
    ;

type:
    TOKEN_TYPE_NUMBER { $$ = strdup("number"); }
    | TOKEN_TYPE_FLOAT  { $$ = strdup("float"); }
    | TOKEN_TYPE_CHAR   { $$ = strdup("char"); }
    | TOKEN_TYPE_STRING { $$ = strdup("string"); }
    | TOKEN_TYPE_BOOL   { $$ = strdup("bool"); }
    ;

var_decl:
    TOKEN_RAKH TOKEN_YAAR type TOKEN_IDENTIFIER TOKEN_ASSIGN expr TOKEN_SEMICOLON
    {
        $$ = new VarDeclNode($3, $4, $6, yylineno);
        free($3);
        free($4);
    }
    ;

list_decl:
    TOKEN_LIST TOKEN_YAAR type TOKEN_IDENTIFIER TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET TOKEN_SEMICOLON
    {
        $$ = new ListDeclNode($3, $4, $6, yylineno);
        free($3);
        free($4);
    }
    ;

assignment_stmt:
    TOKEN_IDENTIFIER TOKEN_ASSIGN expr TOKEN_SEMICOLON
    {
        $$ = new AssignNode($1, nullptr, $3, yylineno);
        free($1);
    }
    | TOKEN_IDENTIFIER TOKEN_LBRACKET expr TOKEN_RBRACKET TOKEN_ASSIGN expr TOKEN_SEMICOLON
    {
        $$ = new AssignNode($1, $3, $6, yylineno);
        free($1);
    }
    ;

io_stmt:
    TOKEN_BOL TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMICOLON
    {
        $$ = new IONode("bol", $4, "", yylineno);
    }
    | TOKEN_PUCH TOKEN_YAAR TOKEN_LPAREN TOKEN_IDENTIFIER TOKEN_RPAREN TOKEN_SEMICOLON
    {
        $$ = new IONode("puch", nullptr, $4, yylineno);
        free($4);
    }
    ;

if_stmt:
    TOKEN_AGAR TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt %prec LOWER_THAN_WARNA
    {
        $$ = new IfNode($4, $6, nullptr, yylineno);
    }
    | TOKEN_AGAR TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt TOKEN_WARNA block_stmt
    {
        $$ = new IfNode($4, $6, $8, yylineno);
    }
    | TOKEN_AGAR TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt elif_list
    {
        IfNode* node = new IfNode($4, $6, nullptr, yylineno);
        if ($7) {
            node->elif_branches = *$7;
            delete $7;
        }
        $$ = node;
    }
    | TOKEN_AGAR TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt elif_list TOKEN_WARNA block_stmt
    {
        IfNode* node = new IfNode($4, $6, $9, yylineno);
        if ($7) {
            node->elif_branches = *$7;
            delete $7;
        }
        $$ = node;
    }
    ;

elif_list:
    elif_list TOKEN_WARNA TOKEN_AGAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt
    {
        $$ = $1;
        $$->push_back({$5, $7});
    }
    | TOKEN_WARNA TOKEN_AGAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt
    {
        $$ = new std::vector<std::pair<ExprNode*, StmtNode*>>();
        $$->push_back({$4, $6});
    }
    ;

switch_stmt:
    TOKEN_DEKH TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_LBRACE case_list default_clause TOKEN_RBRACE
    {
        SwitchNode* sw = new SwitchNode($4, yylineno);
        if ($7) {
            sw->cases = *$7;
            delete $7;
        }
        if ($8) {
            sw->default_statements = *$8;
            delete $8;
        }
        $$ = sw;
    }
    | TOKEN_DEKH TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_LBRACE case_list TOKEN_RBRACE
    {
        SwitchNode* sw = new SwitchNode($4, yylineno);
        if ($7) {
            sw->cases = *$7;
            delete $7;
        }
        $$ = sw;
    }
    ;

case_list:
    case_list case_clause
    {
        $$ = $1;
        $$->push_back($2);
    }
    | /* empty */
    {
        $$ = new std::vector<CaseNode*>();
    }
    ;

case_clause:
    TOKEN_SARDI literal TOKEN_COLON statement_list
    {
        CaseNode* cn = new CaseNode($2, yylineno);
        if ($4) {
            cn->statements = *$4;
            delete $4;
        }
        $$ = cn;
    }
    ;

default_clause:
    TOKEN_GARMI TOKEN_COLON statement_list
    {
        $$ = $3;
    }
    ;

while_stmt:
    TOKEN_JAB TOKEN_TAK TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN block_stmt
    {
        $$ = new WhileNode($5, $7, yylineno);
    }
    ;

for_stmt:
    TOKEN_GHOOM TOKEN_YAAR TOKEN_LPAREN for_init TOKEN_SEMICOLON expr TOKEN_SEMICOLON for_step TOKEN_RPAREN block_stmt
    {
        $$ = new ForNode($4, $6, $8, $10, yylineno);
    }
    ;

for_init:
    TOKEN_RAKH TOKEN_YAAR type TOKEN_IDENTIFIER TOKEN_ASSIGN expr
    {
        $$ = new VarDeclNode($3, $4, $6, yylineno);
        free($3);
        free($4);
    }
    | TOKEN_IDENTIFIER TOKEN_ASSIGN expr
    {
        $$ = new AssignNode($1, nullptr, $3, yylineno);
        free($1);
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

for_step:
    TOKEN_IDENTIFIER TOKEN_ASSIGN expr
    {
        $$ = new AssignNode($1, nullptr, $3, yylineno);
        free($1);
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

dowhile_stmt:
    TOKEN_CHALA TOKEN_YAAR block_stmt TOKEN_JAB TOKEN_TAK TOKEN_YAAR TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMICOLON
    {
        $$ = new DoWhileNode($3, $8, yylineno);
    }
    ;

break_stmt:
    TOKEN_BAS TOKEN_KAR TOKEN_YAAR TOKEN_SEMICOLON
    {
        $$ = new BreakNode(yylineno);
    }
    ;

continue_stmt:
    TOKEN_AGAY TOKEN_CHAL TOKEN_YAAR TOKEN_SEMICOLON
    {
        $$ = new ContinueNode(yylineno);
    }
    ;

func_decl:
    TOKEN_JUGAAD type TOKEN_IDENTIFIER TOKEN_LPAREN param_list TOKEN_RPAREN block_stmt
    {
        std::vector<std::pair<std::string, std::string>> params;
        if ($5) {
            params = *$5;
            delete $5;
        }
        $$ = new FuncDeclNode($2, $3, params, $7, yylineno);
        free($2);
        free($3);
    }
    ;

param_list:
    param_list_nonempty
    | /* empty */
    {
        $$ = new std::vector<std::pair<std::string, std::string>>();
    }
    ;

param_list_nonempty:
    param_list_nonempty TOKEN_COMMA type TOKEN_IDENTIFIER
    {
        $$ = $1;
        $$->push_back({$3, $4});
        free($3);
        free($4);
    }
    | type TOKEN_IDENTIFIER
    {
        $$ = new std::vector<std::pair<std::string, std::string>>();
        $$->push_back({$1, $2});
        free($1);
        free($2);
    }
    ;

return_stmt:
    TOKEN_DE TOKEN_YAAR expr TOKEN_SEMICOLON
    {
        $$ = new ReturnNode($3, yylineno);
    }
    ;

block_stmt:
    TOKEN_LBRACE statement_list TOKEN_RBRACE
    {
        BlockNode* bn = new BlockNode(yylineno);
        if ($2) {
            bn->statements = *$2;
            delete $2;
        }
        $$ = bn;
    }
    ;

expr:
    expr TOKEN_OR expr
    {
        $$ = new BinaryOpNode("||", $1, $3, yylineno);
    }
    | expr TOKEN_AND expr
    {
        $$ = new BinaryOpNode("&&", $1, $3, yylineno);
    }
    | expr TOKEN_EQ expr
    {
        $$ = new BinaryOpNode("==", $1, $3, yylineno);
    }
    | expr TOKEN_NEQ expr
    {
        $$ = new BinaryOpNode("!=", $1, $3, yylineno);
    }
    | expr TOKEN_LT expr
    {
        $$ = new BinaryOpNode("<", $1, $3, yylineno);
    }
    | expr TOKEN_GT expr
    {
        $$ = new BinaryOpNode(">", $1, $3, yylineno);
    }
    | expr TOKEN_LE expr
    {
        $$ = new BinaryOpNode("<=", $1, $3, yylineno);
    }
    | expr TOKEN_GE expr
    {
        $$ = new BinaryOpNode(">=", $1, $3, yylineno);
    }
    | expr TOKEN_PLUS expr
    {
        $$ = new BinaryOpNode("+", $1, $3, yylineno);
    }
    | expr TOKEN_MINUS expr
    {
        $$ = new BinaryOpNode("-", $1, $3, yylineno);
    }
    | expr TOKEN_MULT expr
    {
        $$ = new BinaryOpNode("*", $1, $3, yylineno);
    }
    | expr TOKEN_DIV expr
    {
        $$ = new BinaryOpNode("/", $1, $3, yylineno);
    }
    | expr TOKEN_MOD expr
    {
        $$ = new BinaryOpNode("%", $1, $3, yylineno);
    }
    | TOKEN_NOT expr
    {
        $$ = new UnaryOpNode("!", $2, yylineno);
    }
    | TOKEN_MINUS expr %prec UMINUS
    {
        $$ = new UnaryOpNode("-", $2, yylineno);
    }
    | TOKEN_LPAREN expr TOKEN_RPAREN
    {
        $$ = $2;
    }
    | TOKEN_IDENTIFIER
    {
        $$ = new VariableNode($1, yylineno);
        free($1);
    }
    | TOKEN_IDENTIFIER TOKEN_LBRACKET expr TOKEN_RBRACKET
    {
        $$ = new ArrayAccessNode($1, $3, yylineno);
        free($1);
    }
    | TOKEN_IDENTIFIER TOKEN_LPAREN arg_list TOKEN_RPAREN
    {
        std::vector<ExprNode*> args;
        if ($3) {
            args = *$3;
            delete $3;
        }
        $$ = new FuncCallNode($1, args, yylineno);
        free($1);
    }
    | literal
    {
        $$ = $1;
    }
    ;

arg_list:
    arg_list_nonempty
    | /* empty */
    {
        $$ = new std::vector<ExprNode*>();
    }
    ;

arg_list_nonempty:
    arg_list_nonempty TOKEN_COMMA expr
    {
        $$ = $1;
        $$->push_back($3);
    }
    | expr
    {
        $$ = new std::vector<ExprNode*>();
        $$->push_back($1);
    }
    ;

literal:
    TOKEN_INT_LITERAL
    {
        $$ = new LiteralNode("number", std::to_string($1), yylineno);
    }
    | TOKEN_FLOAT_LITERAL
    {
        $$ = new LiteralNode("float", std::to_string($1), yylineno);
    }
    | TOKEN_CHAR_LITERAL
    {
        $$ = new LiteralNode("char", std::string(1, $1), yylineno);
    }
    | TOKEN_STRING_LITERAL
    {
        $$ = new LiteralNode("string", $1, yylineno);
        free($1);
    }
    | TOKEN_PAKKA
    {
        $$ = new LiteralNode("bool", "pakka", yylineno);
    }
    | TOKEN_TOPI
    {
        $$ = new LiteralNode("bool", "topi", yylineno);
    }
    ;

%%

void yyerror(const char* s) {
    syntax_errors++;
    std::cerr << "Syntax Error: " << s << " at line " << yylineno << std::endl;
}
