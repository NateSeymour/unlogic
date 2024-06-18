/*
    Generates `unlogic::Parser`
*/

%skeleton "lalr1.cc"
%require "3.8"

%define api.token.constructor
%define api.value.type variant
%define api.namespace {unlogic}
%define api.parser.class {Parser}

%code requires
{
    #include <vector>
    #include "parser/Node.h"
    #include "parser/ParserInterface.h"

    namespace unlogic
    {
        class Scanner;
    }
}

%code top
{
    #include <string>
    #include "parser/Scanner.h"

    static unlogic::Parser::symbol_type yylex(unlogic::Scanner &scanner)
    {
        return scanner.get_next_token();
    }
}

// SETUP
%lex-param { unlogic::Scanner &scanner }
%parse-param { unlogic::Prototype *p }
%parse-param { unlogic::Scanner &scanner }

//%locations

// TOKENS
%define api.token.prefix {TOKEN_}

%token <std::string> IDENTIFIER;
%token ASSIGNMENT;
%token <double> NUMBER;
%left BINOP_ADD BINOP_SUB;
%left BINOP_MUL BINOP_DIV;
%right BINOP_EXP;
%token OPEN_PARENTHESIS;
%token CLOSED_PARENTHESIS;
%token DELIMITER;

%type <unlogic::Prototype> named_prototype;
%type <unlogic::Prototype> anonymous_prototype;
%type <unlogic::PrototypeSignature> prototype_signature;
%type <std::vector<std::string>> identifier_list;
%type <std::vector<std::unique_ptr<unlogic::Node>>> argument_list;
%type <std::unique_ptr<unlogic::Node>> expression;
%type <std::unique_ptr<unlogic::Node>> function_call;


// ENTRY
%start program

%%

program
    : named_prototype
    {
        *p = std::move($1);
    }
    | anonymous_prototype
    {
        *p = std::move($1);
    }
    ;

named_prototype
    : prototype_signature ASSIGNMENT expression
    {
        $$.name = $1.name;
        $$.arguments = $1.arguments;
        $$.body = std::move($3);
    }
    ;

anonymous_prototype
    : expression
    {
        $$.name = "__anon";
        $$.anonymous = true;
        $$.body = std::move($1);

        // Run through AST to determine if a named variable is used and add it to the argument list.
        for(auto const &child : $$.body->Children())
        {
            if(child->Type() == NodeType::Variable)
            {
                auto child_node = reinterpret_cast<VariableNode const*>(child);
                $$.arguments.push_back(child_node->identifier_);
            }
        }
    }
    ;

prototype_signature
    : IDENTIFIER OPEN_PARENTHESIS identifier_list CLOSED_PARENTHESIS
    {
        $$.name = $1;
        $$.arguments = $3;
    }
    ;

identifier_list
    : IDENTIFIER
    {
        $$.push_back($1);
    }
    | identifier_list DELIMITER IDENTIFIER
    {
        auto &list = $1;
        list.push_back($3);
        $$ = $1;
    }
    ;

argument_list
    : expression
    {
        $$.push_back(std::move($1));
    }
    | argument_list DELIMITER expression
    {
        auto &list = $1;
        list.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;


function_call
    : IDENTIFIER OPEN_PARENTHESIS argument_list CLOSED_PARENTHESIS
    {
        $$ = std::make_unique<unlogic::CallNode>($1, std::move($3));
    }
    ;

expression
    : NUMBER
    {
        $$ = std::make_unique<unlogic::ConstantNode>($1);
    }
    | IDENTIFIER
    {
        $$ = std::make_unique<unlogic::VariableNode>($1);
    }
    | function_call
    {
        $$ = std::move($1);
    }
    | expression BINOP_ADD expression
    {
        $$ = std::make_unique<unlogic::BinaryOperationNode>(std::move($1), std::move($3), "+");
    }
    | expression BINOP_SUB expression
    {
        $$ = std::make_unique<unlogic::BinaryOperationNode>(std::move($1), std::move($3), "-");
    }
    | expression BINOP_MUL expression
    {
        $$ = std::make_unique<unlogic::BinaryOperationNode>(std::move($1), std::move($3), "*");
    }
    | expression BINOP_DIV expression
    {
        $$ = std::make_unique<unlogic::BinaryOperationNode>(std::move($1), std::move($3), "/");
    }
    | expression BINOP_EXP expression
    {
        $$ = std::make_unique<unlogic::BinaryOperationNode>(std::move($1), std::move($3), "^");
    }
    | OPEN_PARENTHESIS expression CLOSED_PARENTHESIS
    {
        $$ = std::move($2);
    }
    ;

%%

void unlogic::Parser::error(std::string const &message) {}