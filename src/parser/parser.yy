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
    static unlogic::Parser::symbol_type yylex()
    {
    }
}

// TOKENS
%define api.token.prefix {TOKEN_}

// ENTRY
%start function

%%

function: