#include <gtest/gtest.h>
#include <variant>
#include "parser/Parser.h"

enum class TerminalType
{
    NUMBER,
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    SYMBOL,
    END,
};

typedef std::variant<double, std::string> ValueType;

unlogic::Tokenizer<TerminalType, ValueType> tokenizer;

unlogic::DefineTerminal<TerminalType, ValueType, R"(\-?\d+(\.\d+)?)", double> NUMBER(tokenizer, TerminalType::NUMBER, [](auto tok)
{
    return std::stod(std::string(tok.raw));
});

unlogic::DefineTerminal<TerminalType, ValueType, R"(\+|\-|\*|\/)", std::string> OPERATOR(tokenizer, TerminalType::OPERATOR, [](auto tok)
{
    return std::string(tok.raw);
});

unlogic::DefineTerminal<TerminalType, ValueType, R"([a-zA-Z\d]+)", std::string> IDENTIFIER(tokenizer, TerminalType::IDENTIFIER, [](auto tok)
{
    return std::string(tok.raw);
});

unlogic::DefineTerminal<TerminalType, ValueType, R"(\s*$)", double> END(tokenizer, TerminalType::END, [](auto tok) {
    return 0.0;
});

unlogic::NonTerminal<TerminalType, ValueType> expression
    = (expression + OPERATOR + expression)<=>[](auto &$)
    {
        return std::get<double>($[0]) + std::get<double>($[2]);
    }
    | unlogic::ProductionRule(NUMBER)<=>[](auto &$)
    {
        return std::get<double>($[0]);
    }
    | unlogic::ProductionRule(IDENTIFIER)<=>[](auto &$)
    {
        return 0.0;
    }
    ;

unlogic::NonTerminal<TerminalType, ValueType> program
    = (expression + END)<=>[](auto &$)
    {
        return std::get<double>($[0]);
    }
    ;

TEST(Parser, BasicParsing)
{
    unlogic::Parser<TerminalType, ValueType> p(tokenizer, program);

    //ASSERT_EQ(81.5, std::get<double>(result));
}