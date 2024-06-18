%{
    #include <cstdlib>
    #include "parser/Scanner.h"
%}

%option c++
%option nodefault
%option noyywrap
%option yyclass="Scanner"
%option prefix="unlogic_"

/* DEFINITIONS */
WHITESPACE          [\n\t ]
DIGIT               [0-9]
NUMBER              \-?{DIGIT}+(\.{DIGIT}+)?
IDENTIFIER          [a-zA-Z]+
%%

{WHITESPACE}

{NUMBER} {
    return unlogic::Parser::make_NUMBER(strtod(yytext, nullptr));
}

{IDENTIFIER} { 
    return unlogic::Parser::make_IDENTIFIER(yytext); 
}

\:\= { 
    return unlogic::Parser::make_ASSIGNMENT(); 
}

\( { 
    return unlogic::Parser::make_OPEN_PARENTHESIS(); 
}

\) { 
    return unlogic::Parser::make_CLOSED_PARENTHESIS(); 
}

\, { 
    return unlogic::Parser::make_DELIMITER(); 
}

\- {
    return unlogic::Parser::make_BINOP_SUB();
}

\+ {
    return unlogic::Parser::make_BINOP_ADD();
}

\* {
    return unlogic::Parser::make_BINOP_MUL();
}

\/ {
    return unlogic::Parser::make_BINOP_DIV();
}

\^ {
    return unlogic::Parser::make_BINOP_EXP();
}

%%