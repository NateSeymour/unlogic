#include <gtest/gtest.h>
#include <variant>
#include "parser/Parser.h"

enum class TerminalType
{
    NUMBER,
    KEYWORD,
    OPERATOR,
    SYMBOL,
};

typedef std::variant<double, std::string> ValueType;

unlogic::Tokenizer<TerminalType, ValueType> tokenizer;

unlogic::DefineTerminal<TerminalType, ValueType, R"(\-?\d*\.\d*)", double> NUMBER(tokenizer, TerminalType::NUMBER, [](auto tok) {
    return std::stod(std::string(tok.raw));
});

unlogic::DefineTerminal<TerminalType, ValueType, R"(\+|\-|\*|\/)", std::string> OPERATOR(tokenizer, TerminalType::OPERATOR, [](auto tok) {
    return std::string(tok.raw);
});

unlogic::NonTerminal<TerminalType, ValueType> expression
    = (NUMBER + OPERATOR + NUMBER)<=>[](auto &$)
    {
        auto lhs = NUMBER($[0]);
        auto rhs = NUMBER($[2]);
        auto op = OPERATOR($[1]);

        if(op == "+") return lhs + rhs;
        else if(op == "*") return lhs * rhs;
        else if(op == "-") return lhs - rhs;
        else if(op == "/") return lhs / rhs;

        return 0.0;
    }
    | unlogic::ProductionRule(NUMBER)<=>[](auto &$)
    {
        return NUMBER($[0]);
    }
    ;

TEST(Tokenizer, TerminalMatching)
{
    ASSERT_TRUE(NUMBER.Match("-406.78"));
}

TEST(Tokenizer, TerminalValues)
{
    auto token = *NUMBER.Match("104.23");
    ASSERT_EQ(NUMBER(token), 104.23);
}