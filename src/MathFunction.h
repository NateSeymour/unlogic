#ifndef UNLOGIC_MATHFUNCTION_H
#define UNLOGIC_MATHFUNCTION_H

#include <string>
#include <utility>
#include <memory>
#include <map>
#include "TokenViewer.h"
#include "Node.h"

enum class Precedence : std::uint64_t
{
    Minimum,
    Terminator,
    Delimiter,
    Assignment,
    SumDifference,
    ProductQuotient,
    Maximum,
};

class MathFunction
{
    std::string raw_;
    std::unique_ptr<Node> tree_;

    TokenViewer token_viewer_;

    std::map<std::string, Precedence> op_precedence_ = {
            {"+", Precedence::SumDifference},
            {"-", Precedence::SumDifference},
            {"*", Precedence::ProductQuotient},
            {"/", Precedence::ProductQuotient},
            {",", Precedence::Delimiter},
            {";", Precedence::Terminator},
    };

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
     *
     * EXAMPLES:
     * - 2 + 2
     * - 4^5
     * - 3 + 4 * 6
     * - x - 2
     */

    Precedence TokenPrecedence(Token const &token)
    {
        auto value = token.As<std::string>();

        if(this->op_precedence_.contains(value))
        {
            return this->op_precedence_[value];
        }

        return Precedence::Minimum;
    }

    Precedence NextPrecedence()
    {
        auto t = this->token_viewer_.Peek();
        return this->TokenPrecedence(t);
    }

    std::unique_ptr<Node> ParseBinOp(std::unique_ptr<Node> lhs)
    {
        auto op = this->token_viewer_.Consume();
        auto value = op.As<std::string>()[0];
        auto precedence = this->TokenPrecedence(op);

        auto rhs = this->ParseExpression(precedence);

        return std::make_unique<BinaryNode>(std::move(lhs), std::move(rhs), value);
    }

    std::unique_ptr<Node> ParseExpression(Precedence precedence = Precedence::Assignment)
    {
        std::unique_ptr<Node> expression;

        auto t = this->token_viewer_.Consume();
        switch(t.type)
        {
            case TokenType::Number:
            {
                expression = std::make_unique<ConstantNode>(t.As<double>());
                break;
            }

            case TokenType::Identifier:
            {
                // Function Call
                if(this->token_viewer_.Expect(TokenType::LeftParenthesis))
                {
                    this->token_viewer_.Consume(TokenType::LeftParenthesis);

                    std::vector<std::unique_ptr<Node>> parameters;

                    while(true)
                    {
                        parameters.push_back(this->ParseExpression());

                        auto seperator_or_end = this->token_viewer_.ConsumeAnyOf({ TokenType::RightParenthesis, TokenType::Delimiter });

                        if(seperator_or_end.type == TokenType::RightParenthesis)
                        {
                            break;
                        }
                    }

                    expression = std::make_unique<FunctionCallNode>(t.As<std::string>(), std::move(parameters));
                    break;
                }

                // Else identifier
                expression = std::make_unique<IdentifierNode>(t.As<std::string>());
                break;
            }

            default:
            {
                throw std::runtime_error("unexpected token!");
            }
        }

        while(this->NextPrecedence() > precedence)
        {
            if(this->token_viewer_.Expect(TokenType::Operator))
            {
                expression = this->ParseBinOp(std::move(expression));
            }
            else
            {
                throw std::runtime_error("unexpected token!");
            }
        }

        return expression;
    }

    std::unique_ptr<Node> ParseFunctionDefinition()
    {
        auto identifier = this->token_viewer_.Consume(TokenType::Identifier);
        this->token_viewer_.Consume(TokenType::LeftBracket);

        std::vector<std::string> parameters;

        while(true)
        {
            auto parameter = this->token_viewer_.Consume(TokenType::Identifier);

            parameters.push_back(parameter.As<std::string>());

            auto seperator_or_end = this->token_viewer_.ConsumeAnyOf({ TokenType::RightBracket, TokenType::Delimiter });

            if(seperator_or_end.type == TokenType::RightBracket)
            {
                break;
            }
        }

        this->token_viewer_.Consume(TokenType::Assignment);

        std::shared_ptr<Node> function_body = this->ParseExpression();

        return std::make_unique<FunctionDefinitionNode>(identifier.As<std::string>(), std::move(function_body), parameters);
    }

    std::unique_ptr<Node> ParseStatement()
    {
        std::unique_ptr<Node> statement;

        this->token_viewer_.AssertAnyOf({ TokenType::Number, TokenType::Identifier });

        auto next = this->token_viewer_.Peek(1);
        switch(next.type)
        {
            case TokenType::Operator:
            case TokenType::EndOfFile:
            case TokenType::Terminator:
            case TokenType::LeftParenthesis:
            {
                statement = this->ParseExpression();
                break;
            }

            case TokenType::LeftBracket:
            {
                statement = this->ParseFunctionDefinition();
                break;
            }

            default:
            {
                throw std::runtime_error("unexpected token!");
            }
        }

        this->token_viewer_.Assert(TokenType::Terminator);
        this->token_viewer_.Consume();

        return statement;
    }

    std::unique_ptr<Node> ParseProgram()
    {
        auto program = std::make_unique<BlockNode>();

        while(!this->token_viewer_.Expect(TokenType::EndOfFile))
        {
            program->AddStatement(this->ParseStatement());
        }

        return program;
    }

public:
    double Evaluate(EvaluationContext &context)
    {
        return this->tree_->Evaluate(context);
    }

    explicit MathFunction(std::string equation) : raw_(std::move(equation)), token_viewer_(raw_)
    {
        this->tree_ = this->ParseProgram();
    }
};

#endif //UNLOGIC_MATHFUNCTION_H
