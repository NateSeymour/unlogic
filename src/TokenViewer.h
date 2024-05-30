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

namespace unlogic
{
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

    enum class Precedence : std::uint64_t
    {
        Minimum,
        Terminator,
        Delimiter,
        Assignment,
        SumDifference,
        ProductQuotient,
        Power,
        Maximum,
    };

    struct Token
    {
        std::string value;
        TokenType type;

        Precedence Precedence() const;
        std::string const &String() const;
        double Double() const;
    };

    class TokenViewer
    {
        friend class TokenException;

    private:
        Token TokenizeSymbol();
        Token TokenizeIdentifier();
        Token TokenizeNumber();
        Token ParseNextToken();

    protected:
        std::string const &input_;
        std::uint64_t index_ = 0;

        std::vector<Token> tokens_;

    public:
        Token const &Consume(TokenType type = TokenType::Any);
        Token const &ConsumeAnyOf(std::vector<TokenType> const &types);

        Token const &Peek(std::uint64_t lookahead = 0);

        bool Expect(TokenType type, std::uint64_t lookahead = 0);
        bool Expect(TokenType type, std::string const &value, std::uint64_t lookahead = 0);
        bool ExpectAnyOf(std::vector<TokenType> const &types, std::uint64_t lookahead = 0);

        void Assert(TokenType type, std::uint64_t lookahead = 0);
        template<typename T>
        void Assert(TokenType type, T const &value, std::uint64_t lookahead = 0)
        {
            if (!this->Expect(type, value, lookahead)) {
                throw std::runtime_error("unexpected token!");
            }
        }

        void AssertAnyOf(std::vector<TokenType> const &types);

        explicit TokenViewer(std::string const &input) : input_(input)
        {
            while(true)
            {
                Token token = this->ParseNextToken();
                this->tokens_.push_back(token);

                if(token.type == TokenType::EndOfFile)
                {
                    break;
                }
            }

            this->index_ = 0;
        }
    };
}

#endif //UNLOGIC_TOKENVIEWER_H
