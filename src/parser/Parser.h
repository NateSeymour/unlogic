#ifndef UNLOGIC_PARSER_H
#define UNLOGIC_PARSER_H

#include <stack>
#include <vector>
#include <functional>
#include <expected>
#include "Error.h"
#include "parser/Lex.h"

namespace unlogic
{
    template<typename T, typename ValueType>
    class Parser;

    class ParseError : public Error
    {
        /* Empty */
    };

    template<typename T, typename ValueType>
    class ProductionRule
    {
        friend class Parser<T, ValueType>;

    protected:
        T start_terminal_;

    public:
        ProductionRule(T start_terminal) : start_terminal_(start_terminal)
        {

        }
    };

    template<typename T, typename ValueType>
    class ProductionRuleList
    {

    };

    template<typename T, typename ValueType>
    class NonTerminal
    {
        friend class Parser<T, ValueType>;

    protected:
        std::vector<ProductionRule<T, ValueType>> production_rules_;

    public:
        ValueType Parse(Lex<T> &lex) {}

        NonTerminal(ProductionRule<T, ValueType> rule)
        {
        }
    };

    template<typename T, std::size_t TerminalCount = 1>
    class TerminalSequence
    {
        friend class TerminalSequence<T, TerminalCount - 1>;

    protected:
        std::array<TerminalInterface<T> *, TerminalCount> terminals_;

    public:
        static const size_t terminal_count = TerminalCount;

        consteval TerminalInterface<T> *operator[](std::size_t i)
        {
            return this->terminals_[i];
        }

        template<size_t NewTerminalCount = TerminalCount + 1>
        constexpr auto operator+(TerminalInterface<T> &rhs)
        {
            TerminalSequence<T, NewTerminalCount> sequence;

            for(int i = 0; i < TerminalCount; i++)
            {
                sequence.terminals_[i] = this->terminals_[i];
            }
            sequence.terminals_[TerminalCount] = &rhs;

            return std::move(sequence);
        }

        template<typename F, typename ValueType = std::invoke_result<F, std::vector<LexPosition>&>::type>
        ProductionRule<T, ValueType> operator<=>(F transductor)
        {
            return ProductionRule<T, ValueType>(this->terminals_[0]->Type());
        }

        TerminalSequence(TerminalInterface<T> &terminal)
        {
            this->terminals_[0] = &terminal;
        }

        TerminalSequence() = default;
    };

    template<typename T>
    TerminalSequence<T, 2> operator+(TerminalInterface<T> &lhs, TerminalInterface<T> &rhs)
    {
        return TerminalSequence(lhs) + rhs;
    }

    template<typename T, typename ValueType>
    class Parser
    {
        Lex<T> &lex_;
        std::stack<NonTerminal<T, ValueType>> parsing_stack_;

    public:
        std::expected<ValueType, ParseError> Parse()
        {
            auto &nonterminal = this->parsing_stack_.top();
            auto next_token = this->lex_.Next();

            for(auto &production_rule : nonterminal.production_rules_)
            {
                if(!next_token)
                {
                    return std::unexpected("Unexpected end of token stream!");
                }

                if(next_token->type == production_rule.start_terminal)
                {

                }
            }
        }

        Parser(Lex<T> &lex, NonTerminal<T, ValueType> &start) : lex_(lex)
        {
            this->parsing_stack_.push(start);
        }
    };
}

#endif //UNLOGIC_PARSER_H
