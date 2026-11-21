%{
#include <iostream>
#include <string>
#include <cstdlib>
#include "parser.tab.hpp"

extern int yylineno;
void yyerror(const char* s);
%}

%option noyywrap
%option yylineno

/* Regular Expression Macros */
DIGIT           [0-9]
LETTER          [a-zA-Z_]
IDENTIFIER      {LETTER}({LETTER}|{DIGIT})*
INT_CONST       {DIGIT}+
FLOAT_CONST     {DIGIT}+"."{DIGIT}+
CHAR_CONST      \'([^\'\\\n]|(\\.))\'
STRING_CONST    \"([^\"\\\n]|(\\.))*\"
WHITESPACE      [ \t\r]+
NEWLINE         \n

%%

"//".*                  { /* Ignore single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Ignore multi-line comments */ }

"sun"                   { return TOKEN_SUN; }
"khatam"                { return TOKEN_KHATAM; }
"yaar"                  { return TOKEN_YAAR; }
"rakh"                  { return TOKEN_RAKH; }
"number"                { return TOKEN_TYPE_NUMBER; }
"float"                 { return TOKEN_TYPE_FLOAT; }
"char"                  { return TOKEN_TYPE_CHAR; }
"string"                { return TOKEN_TYPE_STRING; }
"bool"                  { return TOKEN_TYPE_BOOL; }
"pakka"                 { return TOKEN_PAKKA; }
"topi"                  { return TOKEN_TOPI; }
"bol"                   { return TOKEN_BOL; }
"puch"                  { return TOKEN_PUCH; }
"agar"                  { return TOKEN_AGAR; }
"warna"                 { return TOKEN_WARNA; }
"dekh"                  { return TOKEN_DEKH; }
"sardi"                 { return TOKEN_SARDI; }
"garmi"                 { return TOKEN_GARMI; }
"ghoom"                 { return TOKEN_GHOOM; }
"jab"                   { return TOKEN_JAB; }
"tak"                   { return TOKEN_TAK; }
"chala"                 { return TOKEN_CHALA; }
"bas"                   { return TOKEN_BAS; }
"kar"                   { return TOKEN_KAR; }
"agay"                  { return TOKEN_AGAY; }
"chal"                  { return TOKEN_CHAL; }
"jugaad"                { return TOKEN_JUGAAD; }
"de"                    { return TOKEN_DE; }
"list"                  { return TOKEN_LIST; }

"=="                    { return TOKEN_EQ; }
"!="                    { return TOKEN_NEQ; }
"<="                    { return TOKEN_LE; }
">="                    { return TOKEN_GE; }
"<"                     { return TOKEN_LT; }
">"                     { return TOKEN_GT; }
"="                     { return TOKEN_ASSIGN; }

"&&"                    { return TOKEN_AND; }
"||"                    { return TOKEN_OR; }
"!"                     { return TOKEN_NOT; }

"+"                     { return TOKEN_PLUS; }
"-"                     { return TOKEN_MINUS; }
"*"                     { return TOKEN_MULT; }
"/"                     { return TOKEN_DIV; }
"%"                     { return TOKEN_MOD; }

"("                     { return TOKEN_LPAREN; }
")"                     { return TOKEN_RPAREN; }
"{"                     { return TOKEN_LBRACE; }
"}"                     { return TOKEN_RBRACE; }
"["                     { return TOKEN_LBRACKET; }
"]"                     { return TOKEN_RBRACKET; }
";"                     { return TOKEN_SEMICOLON; }
","                     { return TOKEN_COMMA; }
":"                     { return TOKEN_COLON; }

{INT_CONST}             { yylval.int_val = std::atoi(yytext); return TOKEN_INT_LITERAL; }
{FLOAT_CONST}           { yylval.float_val = std::atof(yytext); return TOKEN_FLOAT_LITERAL; }
{CHAR_CONST}            { yylval.char_val = yytext[1]; return TOKEN_CHAR_LITERAL; }
{STRING_CONST}          { 
                            std::string s(yytext); 
                            yylval.str_val = strdup(s.substr(1, s.length() - 2).c_str()); 
                            return TOKEN_STRING_LITERAL; 
                        }
{IDENTIFIER}            { yylval.str_val = strdup(yytext); return TOKEN_IDENTIFIER; }

{WHITESPACE}            { /* Ignore whitespace */ }
{NEWLINE}               { /* yylineno handles line count */ }

.                       { 
                            std::cerr << "Lexical Error: Unrecognized character '" << yytext 
                                      << "' at line " << yylineno << std::endl; 
                        }

%%
