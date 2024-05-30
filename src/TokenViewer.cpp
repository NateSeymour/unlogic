//
// Created by nathan on 5/23/24.
//

#include <map>
#include "TokenViewer.h"
#include "Exception.h"

using namespace unlogic;

Token const &TokenViewer::Consume(TokenType type)
{
    this->Assert(type);

    if(this->index_ < this->tokens_.size())
    {
        return this->tokens_[this->index_++];
    }

    throw std::runtime_error("unexpected end of token stream!");
}

Token const &TokenViewer::ConsumeAnyOf(const std::vector<TokenType> &types)
{
    this->AssertAnyOf(types);
    return this->Consume();
}

Token const &TokenViewer::Peek(std::uint64_t lookahead)
{
    if(this->index_ + lookahead < this->tokens_.size())
    {
        return this->tokens_[this->index_ + lookahead];
    }

    throw std::runtime_error("unexpected end of token stream!");
}

bool TokenViewer::Expect(TokenType type, std::uint64_t lookahead)
{
    if(this->index_ + lookahead < this->tokens_.size())
    {
        return this->tokens_[this->index_ + lookahead].type == type;
    }

    return false;
}

void TokenViewer::Assert(TokenType type, std::uint64_t lookahead)
{
    if(type == TokenType::Any) return;

    if(!this->Expect(type, lookahead))
    {
        throw std::runtime_error("token other than expected!");
    }
}

void TokenViewer::AssertAnyOf(const std::vector<TokenType> &types)
{
    for(auto const type : types)
    {
        if(this->Expect(type)) return;
    }

    throw std::runtime_error("token other than expected!");
}

bool TokenViewer::ExpectAnyOf(const std::vector<TokenType> &types, std::uint64_t lookahead)
{
    for(auto const type : types)
    {
        if(this->Expect(type, lookahead)) return true;
    }

    return false;
}

Token TokenViewer::ParseNextToken()
{
    // Cut out whitespace
    while(std::isspace(this->input_[this->index_]))
    {
        this->index_++;
    }

    if(this->index_ >= this->input_.size())
    {
        return {
            .value = "",
            .type = TokenType::EndOfFile,
        };
    }

    if(std::isalpha(this->input_[this->index_]))
    {
        return this->TokenizeIdentifier();
    }

    if(std::isdigit(this->input_[this->index_]))
    {
        return this->TokenizeNumber();
    }

    return this->TokenizeSymbol();
}

Token TokenViewer::TokenizeNumber()
{
    std::string value;

    std::size_t punctuation_count = 0;
    while(char c = this->input_[this->index_])
    {
        if(std::isdigit(c))
        {
            value.push_back(c);
        }
        else if(c == '.')
        {
            if(punctuation_count > 0)
            {
                break;
            }

            punctuation_count++;
        }
        else
        {
            break;
        }

        this->index_++;
    }

    return {
        .value = value,
        .type = TokenType::Number,
    };
}

Token TokenViewer::TokenizeIdentifier()
{
    std::string value;

    while(char c = this->input_[this->index_])
    {
        if(!std::isalnum(c))
        {
            break;
        }

        value.push_back(c);
        this->index_++;
    }

    return {
            .value = value,
            .type = TokenType::Identifier,
    };
}

Token TokenViewer::TokenizeSymbol()
{
    char c = this->input_[this->index_++];
    switch(c)
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
            return {
                .value = std::string(1, c),
                .type = TokenType::Operator,
            };

        case '(':
            return {
                .value = std::string(1, c),
                .type = TokenType::LeftParenthesis,
            };

        case ')':
            return {
                    .value = std::string(1, c),
                    .type = TokenType::RightParenthesis,
            };

        case '[':
            return {
                    .value = std::string(1, c),
                    .type = TokenType::LeftBracket,
            };

        case ']':
            return {
                    .value = std::string(1, c),
                    .type = TokenType::RightBracket,
            };

        case ';':
            return {
                    .value = std::string(1, c),
                    .type = TokenType::Terminator,
            };

        case ',':
            return {
                    .value = std::string(1, c),
                    .type = TokenType::Delimiter,
            };

        case '=':
            return {
                    .value = std::string(1, c),
                    .type = TokenType::Assignment,
            };

        default:
        {
            throw TokenException(*this);
        }
    }
}

bool TokenViewer::Expect(TokenType type, const std::string &value, std::uint64_t lookahead)
{
    return this->Expect(type, lookahead) && this->tokens_[this->index_ + lookahead].String() == value;
}

std::string const &Token::String() const
{
    return this->value;
}

double Token::Double() const
{
    return std::stod(this->value);
}

std::map<std::string, Precedence> op_precedence = {
        {"+", Precedence::SumDifference},
        {"-", Precedence::SumDifference},
        {"*", Precedence::ProductQuotient},
        {"/", Precedence::ProductQuotient},
        {",", Precedence::Delimiter},
        {";", Precedence::Terminator},
        {"^", Precedence::Power},
};

Precedence Token::Precedence() const
{
    if(!op_precedence.contains(this->value))
    {
        return Precedence::Minimum;
    }

    return op_precedence[this->value];
}
