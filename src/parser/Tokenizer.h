#ifndef UNLOGIC_TOKENIZER_H
#define UNLOGIC_TOKENIZER_H

#include <expected>
#include <optional>
#include <string_view>
#include <functional>
#include <vector>
#include <cctype>
#include <ctre.hpp>
#include "Error.h"

namespace unlogic
{
    class LexError : public Error
    {
    public:
        //const LexPosition position;
    };

    template<typename T>
    struct Token
    {
        T type;
        int begin = -1;
        int end = -1;
        std::string_view raw;
    };

    template<typename T, typename ValueType>
    class Terminal
    {
    protected:
        std::function<ValueType(Token<T> const&)> semantic_reasoner_;

    public:
        typedef ValueType value_type;
        const T terminal_type;

        virtual constexpr std::optional<Token<T>> Match(std::string_view input) const = 0;

        Terminal(T terminal_type, std::function<ValueType(Token<T> const&)> semantic_reasoner) : terminal_type(terminal_type), semantic_reasoner_(semantic_reasoner) {}
    };

    template<typename T, typename ValueType>
    class Tokenizer
    {
        std::vector<Terminal<T, ValueType> *> terminals_;

    public:
        void RegisterTerminal(Terminal<T, ValueType> *terminal)
        {
            this->terminals_.push_back(terminal);
        }

        std::expected<std::vector<Token<T>>, Error> Tokenize(std::string_view input)
        {
            std::vector<Token<T>> tokens;
            int index = 0;

            while(!input.empty())
            {
                // Clear whitespace
                while(std::isspace(input[0]))
                {
                    input = input.substr(1);
                    index++;
                }

                // Do terminal matching
                bool matched = false;
                for(auto terminal : this->terminals_)
                {
                    if(auto match = terminal->Match(input))
                    {
                        input = input.substr(match->end);

                        match->begin += index;
                        match->end += index;
                        int match_length = match->end - match->begin;

                        tokens.push_back(*match);

                        matched = true;
                        break;
                    }
                }

                if(!matched)
                {
                    return std::unexpected("Could not match token to any pattern!");
                }
            }

            return tokens;
        }
    };

    template<typename T, typename ValueType, ctll::fixed_string pattern, typename SemanticType>
    class DefineTerminal : public Terminal<T, ValueType>
    {
    public:
        constexpr std::optional<Token<T>> Match(std::string_view input) const override
        {
            auto match = ctre::starts_with<pattern>(input);
            if(match)
            {
                auto raw = match.to_view();

                return Token<T> {
                        .type = this->terminal_type,
                        .begin = 0,
                        .end = (int)raw.size(),
                        .raw = raw,
                };
            }

            return std::nullopt;
        }

        SemanticType operator()(Token<T> const &token)
        {
            return std::get<SemanticType>(this->semantic_reasoner_(token));
        }

        DefineTerminal(Tokenizer<T, ValueType> &tokenizer, T terminal_type, std::function<ValueType(Token<T> const&)> semantic_reasoner) : Terminal<T, ValueType>(terminal_type, semantic_reasoner)
        {
            tokenizer.RegisterTerminal(this);
        }
    };
}

#endif //UNLOGIC_TOKENIZER_H
