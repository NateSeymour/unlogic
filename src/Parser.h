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
     *      | identifier
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

        std::unique_ptr<Node> ParseBinOp(std::unique_ptr<Node> lhs);
        std::unique_ptr<Node> ParseExpression(Precedence precedence = Precedence::Assignment);
        std::unique_ptr<Node> ParseFunctionDefinition();
        std::unique_ptr<Node> ParseStatement();

    public:
        std::unique_ptr<Node> ParseProgram();

        explicit Parser(std::string const &input) : input_(input), tv_(input_) {}
    };

    std::unique_ptr<Node> parse(std::string const &input);
}

#endif //UNLOGIC_PARSER_H
