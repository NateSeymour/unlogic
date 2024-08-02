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

// TODO: append_range instead of insert_range

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
        friend class Parser<T, ValueType>;
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
            //static_assert(false, "unimplemented");
            return std::nullopt;
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
        friend class Parser<T, ValueType>;
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
        friend class Parser<T, ValueType>;

    protected:
        ProductionRuleList<T, ValueType> production_rules_;

    public:
        NonTerminal(ProductionRuleList<T, ValueType> const &production_rules) : production_rules_(production_rules) {}
        NonTerminal(ProductionRule<T, ValueType> const &production_rule) : production_rules_({ production_rule }) {}
    };

    template<typename T, typename ValueType>
    class Parser
    {
        Tokenizer<T, ValueType> const &tokenizer_;
        NonTerminal<T, ValueType> const &start_;

        enum class ActionType
        {
            kShift,
            kReduce,
            kAccept,
        };

        struct Branch
        {
            ProductionRule<T, ValueType> const &rule;
            int index;

            Branch(ProductionRule<T, ValueType> const &rule, int index) : rule(rule), index(index) {};
        };

        struct State
        {
            std::vector<Branch> branches;

            void Append(State const &state)
            {
                this->branches.insert_range(this->branches.end(), state.branches);
            }

            static State Close(NonTerminal<T, ValueType> const &nonterminal, int index = 0)
            {
                State state;

                for(ProductionRule<T, ValueType> const &rule : nonterminal.production_rules_.rules_)
                {
                    state.branches.emplace_back(rule, index);

                    if (rule.parse_sequence_.size() > index)
                    {
                        std::visit(overload{
                            [&](NonTerminal<T, ValueType> *nonterminal)
                            {
                                state.Append(State::Close(*nonterminal));
                            },
                            [](Terminal<T, ValueType> *none) {},
                        }, rule.parse_sequence_[index]);
                    }
                }

                return state;
            }
        };

        std::vector<State> states_;

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
            using LookaheadType = std::variant<Terminal<T, ValueType>*, NonTerminal<T, ValueType>*>;

            // Parser table generation
            this->states_.push_back(State::Close(start));
            for(auto &state : this->states_)
            {
                // Generate list of every possible following symbol
                std::vector<LookaheadType> follows;
                for(auto const &branch : state.branches)
                {
                    if(branch.index < branch.rule.parse_sequence_.size() + 1)
                    {
                        // TODO: generate "shift" rule
                        follows.push_back(branch.rule.parse_sequence_[branch.index]);
                    }
                    else
                    {
                        // TODO: generate "reduce" rule
                    }
                }

                // Generate a new state for each following symbol
                for(auto const &follow : follows)
                {
                    State &follow_state = this->states_.emplace_back();

                    for(auto const &branch : state.branches)
                    {
                        if(branch.rule.parse_sequence_[branch.index] == follow)
                        {
                            Branch &new_branch = follow_state.branches.emplace_back(branch.rule, branch.index + 1);

                            // Close nonterminals if exist at parsing location
                            std::visit(overload{
                                [&](NonTerminal<T, ValueType> *nonterminal)
                                {
                                    follow_state.Append(State::Close(*nonterminal));
                                },
                                [](Terminal<T, ValueType> *none) {},
                            }, new_branch.rule.parse_sequence_[new_branch.index]);
                        }
                    }
                }
            }
        }
    };
}

#endif //UNLOGIC_PARSER_H
