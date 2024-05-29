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
    Any,
    Identifier,
    Number,
    Operator,
    Assignment,
    Terminator,
    LeftParenthesis,
    RightParenthesis,
    LeftBracket,
    RightBracket,
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

    template<typename T> T As() const;
};

class TokenViewer
{
    std::string const &input_;
    std::uint64_t index_ = 0;

    std::vector<Token> tokens_;

    void Process();

public:
    Token const &Consume(TokenType type = TokenType::Any);
    Token const &ConsumeAnyOf(std::vector<TokenType> const &types);
    Token const &Peek(std::uint64_t lookahead = 0);

    bool Expect(TokenType type, std::uint64_t lookahead = 0);
    template<typename T> bool Expect(TokenType type, T const &value, std::uint64_t lookahead = 0)
    {
        return this->Expect(type, lookahead) && this->tokens_[this->index_ + lookahead].As<T>() == value;
    }

    bool ExpectAnyOf(std::vector<TokenType> const &types, std::uint64_t lookahead = 0);

    void Assert(TokenType type, std::uint64_t lookahead = 0);
    template<typename T> void Assert(TokenType type, T const &value, std::uint64_t lookahead = 0)
    {
        if(!this->Expect(type, value, lookahead))
        {
            throw std::runtime_error("unexpected token!");
        }
    }

    void AssertAnyOf(std::vector<TokenType> const &types);

    explicit TokenViewer(std::string const &input) : input_(input)
    {
        this->Process();
    }
};

#endif //UNLOGIC_TOKENVIEWER_H
