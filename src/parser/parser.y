/*
    Generates `unlogic::Parser`
*/

%skeleton "lalr1.cc"
%require "3.8"

%define api.token.constructor
%define api.value.type variant
%define api.namespace {unlogic}
%define api.parser.class {Parser}

%code top
{
    #include <string>
    #include "Scanner.h"
    #include "Node.h"

    static unlogic::Parser::symbol_type yylex(unlogic::Scanner &scanner)
    {
        return scanner.get_next_token();
    }
}

// TOKENS
%define api.token.prefix {TOKEN_}

%token <std::string> IDENTIFIER;

%type <unlogic::Prototype> function;

// ENTRY
%start function

%%

function : IDENTIFIER;

%%