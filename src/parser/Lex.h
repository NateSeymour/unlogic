#ifndef UNLOGIC_LEX_H
#define UNLOGIC_LEX_H

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
    struct LexPosition
    {
        std::string_view raw;
        int begin;
        int end;
    };

    class LexError : public Error
    {
    public:
        //const LexPosition position;
    };

    template<typename T>
    struct LexReturnValue
    {
        T type;
        LexPosition position;
    };

    template<typename T>
    class TerminalInterface
    {
    public:
        virtual constexpr T Type() const = 0;
        virtual constexpr std::optional<LexReturnValue<T>> Match(std::string_view input) const = 0;
    };

    template<typename T>
    class Lex
    {
        int index_ = -1;
        std::optional<std::string_view> input_;
        std::vector<TerminalInterface<T> *> terminals_;

    public:
        void RegisterTerminal(TerminalInterface<T> *terminal)
        {
            this->terminals_.push_back(terminal);
        }

        void SetInput(std::string_view input)
        {
            this->input_ = input;
            this->index_ = 0;
        }

        std::expected<LexReturnValue<T>, LexError> Next(bool peak = false)
        {
            if(!this->input_)
            {
                return std::unexpected(LexError{});
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
                    this->input_ = this->input_->substr(match->position.end);

                    match->position.begin += this->index_;
                    match->position.end += this->index_;
                    int match_length = match->position.end - match->position.begin;

                    if(!peak)
                    {
                        this->index_ += match_length;
                    }

                    return *match;
                }
            }

            return std::unexpected(LexError{});
        }
    };

    template<typename T, T type, ctll::fixed_string pattern, typename ValueType>
    class Terminal : public TerminalInterface<T>
    {
        std::function<ValueType(LexPosition)> semantic_reasoner;

    public:
        typedef ValueType value_type;
        const T terminal_type = type;

        constexpr std::optional<LexReturnValue<T>> Match(std::string_view input) const override
        {
            auto match = ctre::starts_with<pattern>(input);
            if(match)
            {
                auto raw = match.to_view();

                return LexReturnValue<T> {
                    .type = this->terminal_type,
                    .position = {
                        .raw = raw,
                        .begin = 0,
                        .end = (int)raw.size(),
                    },
                };
            }

            return std::nullopt;
        }

        constexpr T Type() const override
        {
            return this->terminal_type;
        }

        constexpr ValueType SemanticValue(LexPosition position)
        {
            return this->semantic_reasoner(position);
        }

        constexpr ValueType operator()(LexPosition position)
        {
            return this->SemanticValue(position);
        }

        Terminal(Lex<T> &lex, std::function<ValueType(LexPosition)> semantic_reasoner) : semantic_reasoner(semantic_reasoner)
        {
            lex.RegisterTerminal(this);
        }
    };
}

#endif //UNLOGIC_LEX_H
