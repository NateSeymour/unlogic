//
// Created by nathan on 5/23/24.
//

#ifndef UNLOGIC_TOKENVIEWER_H
#define UNLOGIC_TOKENVIEWER_H

#include <cstdint>
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>
#include <optional>

enum class TokenType
{
    Identifier,
    Number,
    Operator,
    Assignment,
    Terminator,
    Grouper,
    Delimiter,
    Unexpected,

    EndOfFile,
};

struct Token
{
    std::string const &input;
    std::uint64_t start;
    std::uint64_t end;
    TokenType type;

    template<typename T> T As();
};

class TokenViewer
{
    std::string const &input_;
    std::uint64_t index_ = 0;

    std::vector<Token> tokens_;

    void Process();

public:
    Token const &Consume();
    Token const &Peek(std::uint64_t lookahead = 1);

    bool Expect(TokenType type, std::uint64_t lookahead = 0);
    template<typename T> bool Expect(TokenType type, T const &value, std::uint64_t lookahead = 0)
    {
        return this->Expect(type, lookahead) && this->tokens_[this->index_ + lookahead].As<T>() == value;
    }

    void Assert(TokenType type, std::uint64_t lookahead = 0);
    template<typename T> void Assert(TokenType type, T const &value, std::uint64_t lookahead = 0)
    {
        if(!this->Expect(type, value, lookahead))
        {
            throw std::runtime_error("unexpected token!");
        }
    }

    explicit TokenViewer(std::string const &input) : input_(input)
    {
        this->Process();
    }
};

#endif //UNLOGIC_TOKENVIEWER_H
