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
     * statement:
     *      | statement ';'
     *      | expression
     *      | function_definition '=' expression
     * expression:
     *      | NUMBER
     *      | identifier
     *      | expression BINOP expression
     *      | '(' expression ')'
     *      | identifier '(' parameter_list ')'
     */
    class Parser
    {
        std::string const &input_;
        TokenViewer tv_;

        Precedence NextPrecedence()
        {
            return this->tv_.Peek().Precedence();
        }

        std::unique_ptr<Node> ParseBinOp(std::unique_ptr<Node> lhs);
        std::unique_ptr<Node> ParseExpression(Precedence precedence = Precedence::Assignment);

    public:
        std::unique_ptr<FunctionDefinitionNode> ParseFunctionDefinition();
        std::unique_ptr<Node> ParseStatement();
        std::unique_ptr<Node> ParseProgram();

        explicit Parser(std::string const &input) : input_(input), tv_(input_) {}
    };

    std::unique_ptr<Node> parse(std::string const &input);
}

#endif //UNLOGIC_PARSER_H
