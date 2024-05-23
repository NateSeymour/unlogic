//
// Created by nathan on 5/23/24.
//

#include "TokenViewer.h"

template<> std::string Token::As<std::string>()
{
    return this->input.substr(this->start, this->end - this->start + 1);
}

template<> double Token::As<double>()
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
            continue;
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

            case ';':
                t.type = TokenType::Terminator;
                break;

            case '(':
            case ')':
                t.type = TokenType::Grouper;
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

Token const &TokenViewer::Consume()
{
    if(this->index_ < this->tokens_.size())
    {
        return this->tokens_[this->index_++];
    }

    throw std::runtime_error("unexpected end of token stream!");
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
    if(!this->Expect(type, lookahead))
    {
        throw std::runtime_error("token other than expected!");
    }
}
