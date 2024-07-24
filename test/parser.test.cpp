#include <gtest/gtest.h>
#include <variant>
#include "parser/Tokenizer.h"

enum class TerminalType
{
    NUMBER,
    KEYWORD,
    OPERATOR,
    SYMBOL,
};

typedef std::variant<double, std::string> ValueType;

unlogic::Tokenizer<TerminalType, ValueType> tokenizer;

unlogic::DefineTerminal<TerminalType, ValueType, R"(\-?\d*\.\d*)"> NUMBER(tokenizer, TerminalType::NUMBER, [](auto tok) {
    return std::stod(std::string(tok.raw));
});

unlogic::DefineTerminal<TerminalType, ValueType, R"(\+|\-|\*|\/)"> OPERATOR(tokenizer, TerminalType::OPERATOR, [](auto tok) {
    return std::string(tok.raw);
});

unlogic::DefineNonTerminal<TerminalType, ValueType> expression
    = (NUMBER + OPERATOR + NUMBER)<=>[](auto &$)
    {

    }
    ;

TEST(Tokenizer, TerminalMatching)
{
    ASSERT_TRUE(NUMBER.Match("-406.78"));
}

TEST(Tokenizer, TerminalValues)
{
    auto token = *NUMBER.Match("104.23");
    ASSERT_EQ(std::get<double>(NUMBER(token)), 104.23);
}