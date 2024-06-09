#include "Parser.h"

using namespace unlogic;

std::unique_ptr<Node> Parser::ParseBinaryOperation(std::unique_ptr<Node> lhs)
{
    auto op = this->tv_.Consume();

    auto rhs = this->ParseExpression(op.TokenPrecedence());

    return std::make_unique<BinaryOperationNode>(std::move(lhs), std::move(rhs), op.String());
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

                std::vector<std::unique_ptr<Node>> arguments;

                while (true)
                {
                    arguments.push_back(this->ParseExpression());

                    auto seperator_or_end = this->tv_.ConsumeAnyOf({TokenType::RightParenthesis, TokenType::Delimiter});

                    if (seperator_or_end.type == TokenType::RightParenthesis)
                    {
                        break;
                    }
                }

                expression = std::make_unique<CallNode>(t.String(), std::move(arguments));
                break;
            }

            // Else variable
            expression = std::make_unique<VariableNode>(t.String());
            break;
        }

        default:
        {
            throw std::runtime_error("unexpected token!");
        }
    }

    while (this->tv_.Peek().TokenPrecedence() > precedence)
    {
        if (this->tv_.Expect(TokenType::Operator))
        {
            expression = this->ParseBinaryOperation(std::move(expression));
        }
        else
        {
            throw std::runtime_error("unexpected token!");
        }
    }

    return expression;
}

Prototype Parser::ParseAnonymousFunctionDefinition()
{
    Prototype prototype {
        .name = "__anon",
        .anonymous = true,
    };

    prototype.body = this->ParseExpression();

    // Run through AST to determine if a named variable is used and add it to the argument list.
    for(auto const &child : prototype.body->Children())
    {
        if(child->Type() == NodeType::Variable)
        {
            auto child_node = reinterpret_cast<VariableNode const*>(child);
            prototype.arguments.push_back(child_node->identifier_);
        }
    }

    return prototype;
}

Prototype Parser::ParseNamedFunctionDefinition()
{
    Prototype prototype;

    auto t_prototype_name = this->tv_.Consume(TokenType::Identifier);
    prototype.name = t_prototype_name.String();

    this->tv_.Consume(TokenType::LeftParenthesis);

    while (true)
    {
        auto argument = this->tv_.Consume(TokenType::Identifier);

        prototype.arguments.push_back(argument.String());

        auto seperator_or_end = this->tv_.ConsumeAnyOf({TokenType::RightParenthesis, TokenType::Delimiter});

        if (seperator_or_end.type == TokenType::RightParenthesis)
        {
            break;
        }
    }

    this->tv_.Consume(TokenType::Assignment);

    prototype.body = this->ParseExpression();

    return prototype;
}

Prototype Parser::ParseFunctionDefinition()
{
    if(this->tv_.Expect(TokenType::Identifier) && this->tv_.Expect(TokenType::LeftParenthesis, 1))
    {
        return this->ParseNamedFunctionDefinition();
    }
    else
    {
        return this->ParseAnonymousFunctionDefinition();
    }
}
