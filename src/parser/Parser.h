#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <vector>
#include <optional>
#include <expected>
#include "Error.h"
#include "parser/Tokenizer.h"

namespace unlogic
{
    class ParseError : public Error {};

    // Forward decls
    template<typename T, typename ValueType>
    class NonTerminal;

    template<typename T, typename ValueType>
    class ProductionRule
    {
        friend class NonTerminal<T, ValueType>;
        typedef std::function<ValueType(std::vector<Token<T>>&)> transductor_t;

    protected:
        std::optional<transductor_t> tranductor_ = std::nullopt;

        std::vector<T> terminal_sequence_;

    public:
        std::vector<ProductionRule<T, ValueType>> operator|(ProductionRule<T, ValueType> &rhs)
        {
            return {*this, rhs};
        }

        template<typename F>
        ProductionRule<T, ValueType> &operator<=>(F transductor)
        {
            this->tranductor_ = transductor;
            return *this;
        }

        ProductionRule<T, ValueType> &operator+(Terminal<T, ValueType> &rhs)
        {
            this->terminal_sequence_.push_back(rhs.terminal_type);

            return *this;
        }

        ProductionRule(Terminal<T, ValueType> &start)
        {
            this->terminal_sequence_.push_back(start.terminal_type);
        }
    };

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(Terminal<T, ValueType> &lhs, Terminal<T, ValueType> &rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    std::vector<ProductionRule<T, ValueType>> &operator|(std::vector<ProductionRule<T, ValueType>> &list, ProductionRule<T, ValueType> &rhs)
    {
        list.push_back(rhs);
        return list;
    }

    template<typename T, typename ValueType>
    class NonTerminal
    {
        std::vector<ProductionRule<T, ValueType>> production_rules_;

    public:
        std::expected<ValueType, ParseError> Parse(Tokenizer<T, ValueType> &tokenizer)
        {
            auto token = tokenizer.Next();

        }

        NonTerminal(ProductionRule<T, ValueType> const &production_rule) : production_rules_({ production_rule }) {}
        NonTerminal(std::vector<ProductionRule<T, ValueType>> const &production_rules) : production_rules_(production_rules) {}
    };
}

#endif //UNLOGIC_PARSER_H
