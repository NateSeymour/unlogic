#ifndef UNLOGIC_LEX_H
#define UNLOGIC_LEX_H

#include <expected>
#include <optional>
#include <string_view>
#include <functional>
#include <vector>
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
        const LexPosition position;
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
        virtual constexpr std::optional<LexReturnValue<T>> Match(std::string_view input) const = 0;
    };

    template<typename T>
    class Lex
    {
        std::vector<TerminalInterface<T> *> terminals_;

    public:
        void RegisterTerminal(TerminalInterface<T> *terminal)
        {
            this->terminals_.push_back(terminal);
        }

        std::expected<LexReturnValue<T>, LexError> Next() {}
    };

    template<typename T, T type, ctll::fixed_string pattern, typename ValueType>
    class Terminal : public TerminalInterface<T>
    {
        std::function<std::expected<ValueType, LexError>(LexPosition)> semantic_reasoner;

    public:
        static const T terminal_type = type;

        constexpr std::optional<LexReturnValue<T>> Match(std::string_view input) const override
        {
            auto match = ctre::starts_with<pattern>(input);
            if(match)
            {
                LexReturnValue<T> return_value {
                    .type = this->terminal_type,
                    .position = {
                        .raw = match.to_view(),
                        .begin = 0,
                        .end = 0,
                    },
                };
            }

            return std::nullopt;
        }

        Terminal(Lex<T> &lex, std::function<std::expected<ValueType, LexError>(LexPosition)> semantic_reasoner) : semantic_reasoner(semantic_reasoner)
        {
            lex.RegisterTerminal(this);
        }
    };
}

#endif //UNLOGIC_LEX_H
