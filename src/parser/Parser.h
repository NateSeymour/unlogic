#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <vector>
#include <deque>
#include <optional>
#include <expected>
#include <span>
#include <ranges>
#include <string_view>
#include <functional>
#include <cctype>
#include <variant>
#include <stack>
#include <ctre.hpp>
#include "Error.h"

namespace unlogic
{
#pragma region Forwards Decls
    template<typename T, typename ValueType>
    class Parser;

    template<typename T, typename ValueType>
    class NonTerminal;

    template<typename T, typename ValueType>
    class ProductionRule;
#pragma endregion
#pragma region std::visit Hackery
    /**
     * Helper for std::visit provided by Andreas Fertig.
     * Apple-Clang 15 isn't C++23 compliant enough for the prettier solution, so C++17 style it is.
     * https://andreasfertig.blog/2023/07/visiting-a-stdvariant-safely/
     * @tparam Ts
     */
    template<class...>
    constexpr bool always_false_v = false;

    template<class... Ts>
    struct overload : Ts...
    {
        using Ts::operator()...;

        // Prevent implicit type conversions
        template<typename T>
        constexpr void operator()(T) const
        {
            static_assert(always_false_v<T>, "Unsupported type");
        }
    };

    template<class... Ts>
    overload(Ts...) -> overload<Ts...>;
#pragma endregion

    // Error types
    class ParseError : public Error {};
    class LexError : public Error
    {
    public:
        LexError(char const *error) : Error(error) {}
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
    protected:
        std::vector<Terminal<T, ValueType> *> terminals_;

    public:
        class TokenStream
        {
            Tokenizer<T, ValueType> const &tokenizer_;
            std::string_view input_;
            std::deque<Token<T>> buffer_;
            std::size_t index_ = 0;

        public:
            std::expected<Token<T>, LexError> ReadNext()
            {
                // Clear whitespace
                while(std::isspace(this->input_[0]))
                {
                    this->input_ = this->input_.substr(1);
                    this->index_++;
                }

                // Do terminal matching
                for(auto terminal : this->tokenizer_.terminals_)
                {
                    if(auto match = terminal->Match(this->input_))
                    {
                        this->input_ = this->input_.substr(match->end);

                        match->begin += this->index_;
                        match->end += this->index_;
                        int match_length = match->end - match->begin;

                        return *match;
                    }
                }

                return std::unexpected("End of token stream!");
            }

            std::expected<Token<T>, LexError> Consume()
            {
                if(!this->buffer_.empty())
                {
                    Token<T> token = this->buffer_[0];
                    this->buffer_.pop_front();

                    return token;
                }

                return this->ReadNext();
            }

            std::expected<Token<T>, LexError> Peek(std::size_t lookahead = 0)
            {
                // Fill buffer to desired location
                while(this->buffer_.size() <= lookahead)
                {
                    this->buffer_.push_back(*this->ReadNext());
                }

                return this->buffer_[lookahead];
            }

            TokenStream(Tokenizer<T, ValueType> const &tokenizer, std::string_view input) : tokenizer_(tokenizer), input_(input) {}
        };

        friend class Tokenizer<T, ValueType>::TokenStream;

        void RegisterTerminal(Terminal<T, ValueType> *terminal)
        {
            this->terminals_.push_back(terminal);
        }

        TokenStream Stream(std::string input)
        {
            return TokenStream(*this, input);
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

        std::optional<ValueType> Produce(Tokenizer<T, ValueType>::TokenStream &stream)
        {
            // If we have no transductor then there is no point in attempting parse.
            if(!this->tranductor_)
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

                        if(stream.Peek()->type == terminal->terminal_type)
                        {
                            return terminal->semantic_reasoner_(*stream.Consume());
                        }

                        return std::nullopt;
                    }
                    else
                    {
                        NonTerminal<T, ValueType> *nonterminal = val;
                        return *nonterminal->Parse(stream);
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

#pragma region ProductionRule Composition Functions
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
#pragma endregion

    template<typename T, typename ValueType>
    class NonTerminal
    {
    protected:
        ProductionRuleList<T, ValueType> production_rules_;

        std::vector<T> first_;
        std::vector<T> follow_;

        std::vector<T> GenerateFirstSet() const
        {
            std::vector<T> first;

            for(auto const &rule : this->production_rules_.rules_)
            {
                std::visit(overload{
                    [&](Terminal<T, ValueType> *terminal)
                    {
                        first.push_back(terminal->terminal_type);
                    },
                    [&](NonTerminal<T, ValueType> *nonterminal)
                    {
                        // Normally really dumb, but _should_ work within the constraints of the project.
                        if(nonterminal == this) return;

                        first.insert_range(first.end(), nonterminal->first_);
                    },
                }, rule.parse_sequence_[0]);
            }

            return first;
        }

    public:
        std::expected<ValueType, Error> Parse(Tokenizer<T, ValueType>::TokenStream &stream)
        {
            auto start = stream.Peek();
            if(!start)
            {
                return std::unexpected("Unexpected end of token stream!");
            }

            for(auto &rule : this->production_rules_.rules_)
            {
                auto value = rule.Produce(stream);

                if(value)
                {
                    return *value;
                }
            }

            return std::unexpected("No matching rules!");
        }

        NonTerminal(ProductionRuleList<T, ValueType> const &production_rules) : production_rules_(production_rules), first_(std::move(this->GenerateFirstSet())) {}
        NonTerminal(ProductionRule<T, ValueType> const &production_rule) : production_rules_({ production_rule }), first_(std::move(this->GenerateFirstSet())) {}
    };

    template<typename T, typename ValueType>
    class Parser
    {
        Tokenizer<T, ValueType> const &tokenizer_;
        NonTerminal<T, ValueType> const &start_;

        // void goto_;
        // void action_;

    public:
        /**
         * @param input
         * @return
         */
        std::expected<ValueType, ParseError> Parse(std::string_view input) const
        {
            using StackType = std::variant<Token<T>, NonTerminal<T, ValueType>>;

            auto stream = this->tokenizer_.Stream(input);
            std::stack<StackType> stack;

            // to shift we call stream.Next();
        }

        Parser(Tokenizer<T, ValueType> const &tokenizer, NonTerminal<T, ValueType> const &start) : tokenizer_(tokenizer), start_(start)
        {
            // Generate LR(1) automaton

        }
    };
}

#endif //UNLOGIC_PARSER_H
