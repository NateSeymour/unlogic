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
        typedef ValueType value_type;
        const T terminal_type_;

        std::function<ValueType(Token<T>&)> semantic_reasoner_;

        Terminal(T terminal_type, std::function<ValueType(Token<T>&)> semantic_reasoner) : terminal_type_(terminal_type), semantic_reasoner_(semantic_reasoner) {}

    public:
        virtual constexpr std::optional<Token<T>> Match(std::string_view input) const = 0;

        constexpr ValueType operator()(Token<T> &token)
        {
            return this->semantic_reasoner_(token);
        }
    };

    template<typename T, typename ValueType>
    class Tokenizer
    {
        int index_ = -1;
        std::optional<std::string_view> input_;
        std::vector<Terminal<T, ValueType> *> terminals_;

    public:
        void RegisterTerminal(Terminal<T, ValueType> *terminal)
        {
            this->terminals_.push_back(terminal);
        }

        void SetInput(std::string_view input)
        {
            this->input_ = input;
            this->index_ = 0;
        }

        std::expected<Token<T>, LexError> Next(bool peak = false)
        {
            if(!this->input_)
            {
                return std::unexpected("No input provided!");
            }

            while(std::isspace(this->input_.value()[0]))
            {
                this->input_ = this->input_->substr(1);
                this->index_++;
            }

            for(auto terminal : this->terminals_)
            {
                if(auto match = terminal->Match(this->input_.value()))
                {
                    this->input_ = this->input_->substr(match->end);

                    match->begin += this->index_;
                    match->end += this->index_;
                    int match_length = match->end - match->begin;

                    if(!peak)
                    {
                        this->index_ += match_length;
                    }

                    return *match;
                }
            }

            return std::unexpected("Unknown error!");
        }
    };

    template<typename T, typename ValueType, ctll::fixed_string pattern>
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
                        .type = this->terminal_type_,
                        .begin = 0,
                        .end = (int)raw.size(),
                        .raw = raw,
                };
            }

            return std::nullopt;
        }

        DefineTerminal(Tokenizer<T, ValueType> &tokenizer, T terminal_type, std::function<ValueType(Token<T> &)> semantic_reasoner) : Terminal<T, ValueType>(terminal_type, semantic_reasoner)
        {
            tokenizer.RegisterTerminal(this);
        }
    };
}

#endif //UNLOGIC_TOKENIZER_H
