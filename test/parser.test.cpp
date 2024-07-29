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

unlogic::NonTerminal<TerminalType, ValueType> expression
    = unlogic::ProductionRule(NUMBER)<=>[](auto &$)
    {
        return NUMBER($[0]);
    }
    | unlogic::ProductionRule(IDENTIFIER)<=>[](auto &$)
    {
        return 0.0;
    }
    | (expression + OPERATOR + expression)<=>[](auto &$)
    {
        return std::get<double>($[0]) + std::get<double>($[2]);
    }
    ;

TEST(Tokenizer, TerminalMatching)
{
    ASSERT_TRUE(NUMBER.Match("-406.78"));
}

TEST(Tokenizer, BasicTokenizing)
{
    auto tokens = *tokenizer.Tokenize("39.2 + 45 * 6");

    ASSERT_EQ(tokens[0].type, TerminalType::NUMBER);
    ASSERT_EQ(tokens[1].type, TerminalType::OPERATOR);
    ASSERT_EQ(tokens[2].type, TerminalType::NUMBER);
    ASSERT_EQ(tokens[3].type, TerminalType::OPERATOR);
    ASSERT_EQ(tokens[4].type, TerminalType::NUMBER);
}

TEST(Parser, BasicParsing)
{
    auto tokens = *tokenizer.Tokenize("39.5 + 42");
    auto result = *expression.Parse(tokens);

    ASSERT_EQ(81.5, std::get<double>(result));
}