#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <string>
#include <utility>
#include <memory>
#include <map>
#include "TokenViewer.h"
#include "Node.h"

namespace unlogic
{
    /*
     * Grammar
     * identifier_list:
     *      | IDENTIFIER
     *      | identifier_list ','
     * parameter_list:
     *      | expression
     *      | identifier_list ','
     * function_definition:
     *      | identifier '[' identifier_list ']'
     * binary_operation:
     *      | expression BINOP expression
     * variable_definition:
     *      | IDENTIFIER ':=' expression
     * variable_declaration:
     *      | 'given' variable_definition
     * expression:
     *      | NUMBER
     *      | IDENTIFIER
     *      | binary_operation
     *      | '(' expression ')'
     *      | identifier '(' parameter_list ')'
     * statement:
     *      | statement ';'
     *      | expression
     *      | function_definition '=' expression
     * program:
     *      | statement program
     *      | ''
     */
    class Parser
    {
        std::string const &input_;
        TokenViewer tv_;

        std::unique_ptr<Node> ParseBinaryOperation(std::unique_ptr<Node> lhs);
        std::unique_ptr<Node> ParseExpression(Precedence precedence = Precedence::Assignment);

        Prototype ParseAnonymousFunctionDefinition();
        Prototype ParseNamedFunctionDefinition();

    public:
        /**
         *
         * @return
         * @grammar
         * function_body:
         *      | expression
         * function_definition:
         *      | IDENTIFIER '(' identifier_list ')' '=' function_body
         */
        Prototype ParseFunctionDefinition();

        explicit Parser(std::string const &input) : input_(input), tv_(input_) {}
    };
}

#endif //UNLOGIC_PARSER_H
