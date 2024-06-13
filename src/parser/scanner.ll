%{
%}

%option c++
%option nodefault
%option noyywrap
%option yyclass="Scanner"

/* DEFINITIONS */
DIGIT   [0-9]
NUMBER  \-{DIGIT}+(\.{DIGIT}+)?

%%

/* RULES */

%%