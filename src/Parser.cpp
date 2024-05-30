#include "Parser.h"

using namespace unlogic;

std::unique_ptr<Node> Parser::ParseBinOp(std::unique_ptr<Node> lhs)
{
    auto op = this->tv_.Consume();
    auto value = op.String()[0];

    auto rhs = this->ParseExpression(op.Precedence());

    return std::make_unique<BinaryNode>(std::move(lhs), std::move(rhs), value);
}

std::unique_ptr<Node> Parser::ParseExpression(Precedence precedence)
{
    std::unique_ptr<Node> expression;

    auto t = this->tv_.Consume();
    switch (t.type)
    {
        case TokenType::Number:
        {
            expression = std::make_unique<ConstantNode>(t.Double());
            break;
        }

        case TokenType::Identifier:
        {
            // Function Call
            if (this->tv_.Expect(TokenType::LeftParenthesis))
            {
                this->tv_.Consume(TokenType::LeftParenthesis);

                std::vector<std::unique_ptr<Node>> parameters;

                while (true)
                {
                    parameters.push_back(this->ParseExpression());

                    auto seperator_or_end = this->tv_.ConsumeAnyOf({TokenType::RightParenthesis, TokenType::Delimiter});

                    if (seperator_or_end.type == TokenType::RightParenthesis)
                    {
                        break;
                    }
                }

                expression = std::make_unique<FunctionCallNode>(t.String(), std::move(parameters));
                break;
            }

            // Else identifier
            expression = std::make_unique<IdentifierNode>(t.String());
            break;
        }

        default:
        {
            throw std::runtime_error("unexpected token!");
        }
    }

    while (this->NextPrecedence() > precedence)
    {
        if (this->tv_.Expect(TokenType::Operator))
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

std::unique_ptr<FunctionDefinitionNode> Parser::ParseFunctionDefinition()
{
    auto identifier = this->tv_.Consume(TokenType::Identifier);
    this->tv_.Consume(TokenType::LeftBracket);

    std::vector<std::string> parameters;

    while (true)
    {
        auto parameter = this->tv_.Consume(TokenType::Identifier);

        parameters.push_back(parameter.String());

        auto seperator_or_end = this->tv_.ConsumeAnyOf({TokenType::RightBracket, TokenType::Delimiter});

        if (seperator_or_end.type == TokenType::RightBracket)
        {
            break;
        }
    }

    this->tv_.Consume(TokenType::Assignment);

    std::shared_ptr<Node> function_body = this->ParseExpression();

    return std::make_unique<FunctionDefinitionNode>(identifier.String(), std::move(function_body), parameters);
}

std::unique_ptr<Node> Parser::ParseStatement()
{
    std::unique_ptr<Node> statement;

    this->tv_.AssertAnyOf({TokenType::Number, TokenType::Identifier});

    auto next = this->tv_.Peek(1);
    switch (next.type)
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

    this->tv_.Assert(TokenType::Terminator);
    this->tv_.Consume();

    return statement;
}

std::unique_ptr<Node> Parser::ParseProgram()
{
    auto program = std::make_unique<BlockNode>();

    while (!this->tv_.Expect(TokenType::EndOfFile))
    {
        program->AddStatement(this->ParseStatement());
    }

    return program;
}

std::unique_ptr<Node> parse(const std::string &input)
{
    Parser p(input);
    return p.ParseProgram();
}
