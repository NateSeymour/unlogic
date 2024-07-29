#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <vector>
#include <optional>
#include <expected>
#include <span>
#include <ranges>
#include <string_view>
#include <functional>
#include <cctype>
#include <variant>
#include <ctre.hpp>
#include "Error.h"

namespace unlogic
{
    // Forward decls
    template<typename T, typename ValueType>
    class NonTerminal;

    template<typename T, typename ValueType>
    class ProductionRule;

    // Error types
    class ParseError : public Error {};
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
        friend class ProductionRule<T, ValueType>;

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

        SemanticType operator()(ValueType const &value)
        {
            return std::get<SemanticType>(value);
        }

        DefineTerminal(Tokenizer<T, ValueType> &tokenizer, T terminal_type, std::function<ValueType(Token<T> const&)> semantic_reasoner) : Terminal<T, ValueType>(terminal_type, semantic_reasoner)
        {
            tokenizer.RegisterTerminal(this);
        }
    };

    template<typename T, typename ValueType>
    class ProductionRule
    {
        friend class NonTerminal<T, ValueType>;
        typedef std::function<ValueType(std::vector<ValueType> const&)> transductor_t;

    protected:
        std::optional<transductor_t> tranductor_ = std::nullopt;

        std::vector<std::variant<Terminal<T, ValueType>*, NonTerminal<T, ValueType>*>> parse_sequence_;

    public:
        template<typename F>
        ProductionRule<T, ValueType> &operator<=>(F transductor)
        {
            this->tranductor_ = transductor;
            return *this;
        }

        ProductionRule<T, ValueType> &operator+(Terminal<T, ValueType> &rhs)
        {
            this->parse_sequence_.push_back(&rhs);

            return *this;
        }

        ProductionRule<T, ValueType> &operator+(NonTerminal<T, ValueType> &rhs)
        {
            this->parse_sequence_.push_back(&rhs);

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

            std::vector<ValueType> values;
            for(int i = 0; i < this->parse_sequence_.size(); i++)
            {
                std::optional<ValueType> value = std::visit([&](auto &&val) -> std::optional<ValueType>
                {
                    using VariantType = std::decay_t<decltype(val)>;

                    if constexpr (std::is_same_v<VariantType, Terminal<T, ValueType>*>)
                    {
                        Terminal<T, ValueType> *terminal = val;

                        auto &token = tokens[i + index];
                        if(token.type == terminal->terminal_type)
                        {
                            return terminal->semantic_reasoner_(token);
                        }

                        return std::nullopt;
                    }
                    else
                    {
                        NonTerminal<T, ValueType> *nonterminal = val;
                        return *nonterminal->Parse(tokens, index + i);
                    }
                }, this->parse_sequence_[i]);

                if(value)
                {
                    values.push_back(*value);
                }
                else
                {
                    return std::nullopt;
                }
            }

            return (*this->tranductor_)(values);
        }

        ProductionRule(Terminal<T, ValueType> &start)
        {
            this->parse_sequence_.push_back(&start);
        }

        ProductionRule(NonTerminal<T, ValueType> &start)
        {
            this->parse_sequence_.push_back(&start);
        }
    };

    template<typename T, typename ValueType>
    class ProductionRuleList
    {
        friend class NonTerminal<T, ValueType>;

    protected:
        std::vector<ProductionRule<T, ValueType>> rules_;

    public:
        ProductionRuleList<T, ValueType> &operator|(ProductionRule<T, ValueType> const &rhs)
        {
            this->rules_.push_back(rhs);
            return *this;
        }

        ProductionRuleList(std::initializer_list<ProductionRule<T, ValueType>> const &rules)
        {
            for(auto const &rule : rules)
            {
                this->rules_.push_back(rule);
            }
        }
    };

    template<typename T, typename ValueType>
    ProductionRuleList<T, ValueType> operator+(Terminal<T, ValueType> &lhs, Terminal<T, ValueType> &rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(Terminal<T, ValueType> &lhs, NonTerminal<T, ValueType> *rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(NonTerminal<T, ValueType> &lhs, NonTerminal<T, ValueType> &rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRule<T, ValueType> operator+(NonTerminal<T, ValueType> &lhs, Terminal<T, ValueType> &rhs)
    {
        return ProductionRule(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    ProductionRuleList<T, ValueType> operator|(ProductionRule<T, ValueType> const &lhs, ProductionRule<T, ValueType> const &rhs)
    {
        return {lhs, rhs};
    }

    template<typename T, typename ValueType>
    class NonTerminal
    {
        int const lookahead_ = 1;
        ProductionRuleList<T, ValueType> production_rules_;

    public:
        std::expected<ValueType, Error> Parse(std::vector<Token<T>> const &tokens, int index = 0)
        {
            for(auto &rule : production_rules_.rules_)
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
        NonTerminal(ProductionRuleList<T, ValueType> const &production_rules) : production_rules_(production_rules) {}
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
