#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <vector>
#include <optional>
#include <expected>
#include <span>
#include <ranges>
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
        typedef std::function<ValueType(std::vector<Token<T>> const&)> transductor_t;

    protected:
        std::optional<transductor_t> tranductor_ = std::nullopt;

        std::vector<std::variant<T, NonTerminal<T, ValueType>*>> parse_sequence_;

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
            this->parse_sequence_.push_back(rhs.terminal_type);

            return *this;
        }

        ProductionRule<T, ValueType> &operator+(NonTerminal<T, ValueType> *rhs)
        {
            this->parse_sequence_.push_back(rhs);

            return *this;
        }

        std::optional<ValueType> Produce(std::vector<Token<T>> const &tokens, int index = 0)
        {
            // If we have no transductor then there is no point in attempting parse.
            if(!this->tranductor_)
            {
                return std::nullopt;
            }

            // Not enough tokens to complete parse
            if(tokens.size() - index < this->parse_sequence_.size())
            {
                return std::nullopt;
            }

            for(int i = 0; i < this->parse_sequence_.size(); i++)
            {
                if(this->parse_sequence_[i] != tokens[index + i].type)
                {
                    return std::nullopt;
                }
            }

            auto &transductor = *this->tranductor_;
            std::vector<Token<T>> temporary(tokens.begin() + index, tokens.end());
            return transductor(temporary);
        }

        ProductionRule(Terminal<T, ValueType> &start)
        {
            this->parse_sequence_.push_back(start.terminal_type);
        }

        ProductionRule(NonTerminal<T, ValueType> *start)
        {
            this->parse_sequence_.push_back(start);
        }
    };

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(Terminal<T, ValueType> &lhs, Terminal<T, ValueType> &rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(Terminal<T, ValueType> &lhs, NonTerminal<T, ValueType> *rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(NonTerminal<T, ValueType> *lhs, NonTerminal<T, ValueType> *rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(NonTerminal<T, ValueType> *lhs, Terminal<T, ValueType> &rhs)
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
        int const lookahead_ = 1;
        std::vector<ProductionRule<T, ValueType>> production_rules_;

    public:
        std::expected<ValueType, Error> Parse(std::vector<Token<T>> const &tokens, int index = 0)
        {
            for(auto &rule : production_rules_)
            {
                auto value = rule.Produce(tokens, index);

                if(value)
                {
                    return *value;
                }
            }

            return std::unexpected("Failed to parse sequence of tokens!");
        }

        NonTerminal(ProductionRule<T, ValueType> const &production_rule) : production_rules_({ production_rule }) {}
        NonTerminal(std::vector<ProductionRule<T, ValueType>> const &production_rules) : production_rules_(production_rules) {}
    };

    template<typename T, typename ValueType>
    class Parser
    {
        Tokenizer<T, ValueType> const &tokenizer_;
        NonTerminal<T, ValueType> const &start_;

    public:
        std::optional<ValueType> Parse(std::string_view input)
        {
            auto tokens = this->tokenizer_.Tokenize(input);
            return this->start_.Parse(*tokens);
        }

        Parser(Tokenizer<T, ValueType> const &tokenizer, NonTerminal<T, ValueType> const &start) : tokenizer_(tokenizer), start_(start) {}
    };
}

#endif //UNLOGIC_PARSER_H
