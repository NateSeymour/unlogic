//
// Created by nathan on 5/23/24.
//

#include "TokenViewer.h"

template<> std::string Token::As<std::string>() const
{
    return this->input.substr(this->start, this->end - this->start);
}

template<> double Token::As<double>() const
{
    return std::stod(this->As<std::string>());
}

void TokenViewer::Process()
{
    std::uint64_t index = 0;

    while(true)
    {
        // Ignore whitespace
        while(std::isspace(this->input_[index]) && index < this->input_.length())
        {
            index++;
        }

        Token t{this->input_};
        t.start = index;

        if(index >= this->input_.length())
        {
            t.end = index;
            t.type = TokenType::EndOfFile;
            this->tokens_.push_back(t);
            break;
        }

        // Identifier
        if(std::isalpha(this->input_[index]))
        {
            while(std::isalpha(this->input_[++index]) && index < this->input_.length());

            t.end = index;
            t.type = TokenType::Identifier;
            this->tokens_.push_back(t);
            continue;
        }

        // Number
        if(std::isdigit(this->input_[index]))
        {
            while(std::isdigit(this->input_[++index]) && index < this->input_.length());

            t.end = index;
            t.type = TokenType::Number;
            this->tokens_.push_back(t);
            continue;
        }

        // Misc.
        switch(this->input_[index++])
        {
            case '+':
            case '-':
            case '*':
            case '/':
                t.type = TokenType::Operator;
                break;

            case '=':
                t.type = TokenType::Assignment;
                break;

            case '\n':
            case ';':
                t.type = TokenType::Terminator;
                break;

            case '[':
                t.type = TokenType::LeftBracket;
                break;

            case ']':
                t.type = TokenType::RightBracket;
                break;

            case '(':
                t.type = TokenType::LeftParenthesis;
                break;

            case ')':
                t.type = TokenType::RightParenthesis;
                break;

            case ',':
                t.type = TokenType::Delimiter;
                break;
        }

        t.end = index;
        this->tokens_.push_back(t);

        if(t.type == TokenType::EndOfFile)
        {
            break;
        }
    }
}

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
