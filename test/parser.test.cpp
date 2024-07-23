#include <gtest/gtest.h>
#include "parser/Lex.h"
#include "parser/Parser.h"

enum class TerminalType
{
    NUMBER,
    KEYWORD,
    OPERATOR,
    SYMBOL,
    UNKNOWN,
};

unlogic::Lex<TerminalType> lex;

unlogic::Terminal<TerminalType, TerminalType::NUMBER, R"(\-?\d*\.\d*)", double> NUMBER(lex, [](auto pos) {
    return std::stod(std::string(pos.raw));
});

unlogic::Terminal<TerminalType, TerminalType::KEYWORD, R"(given|plot|calculate|for|on)", std::string_view> KEYWORD(lex, [](auto pos) {
    return pos.raw;
});

unlogic::Terminal<TerminalType, TerminalType::OPERATOR, R"(\+|\-|\*|\/)", char> OPERATOR(lex, [](auto pos) {
    return pos.raw[0];
});

unlogic::Terminal<TerminalType, TerminalType::SYMBOL, R"(;|\(|\))", std::string_view> SYMBOL(lex, [](auto pos) {
    return pos.raw;
});

unlogic::NonTerminal<TerminalType, double> expression
    = (NUMBER + OPERATOR + NUMBER)<=>[](auto &$) -> double
    {
        return NUMBER($[0]) + NUMBER($[2]);
    }
    /*
    | (SYMBOL + expression + SYMBOL)<=>[](auto a)
    {
        return a[1];
    }
     */
/*
| (NUMBER + OPERATOR["+"] + NUMBER)<=>[](auto a) { return a[0] + a[2]; }
| (NUMBER + OPERATOR["-"] + NUMBER)<=>[](auto a) { return a[0] - a[2]; }
| (NUMBER + OPERATOR["/"] + NUMBER)<=>[](auto a) { return a[0] / a[2]; }
| (NUMBER + OPERATOR["*"] + NUMBER)<=>[](auto a) { return a[0] * a[2]; }
*/
    ;

unlogic::NonTerminal<TerminalType, double> program
    = expression
    ;

TEST(Lex, TerminalMatching)
{
    auto number_match = NUMBER.Match("-305.772");
    ASSERT_TRUE(number_match);
    ASSERT_EQ(number_match->type, TerminalType::NUMBER);
    ASSERT_EQ(NUMBER.SemanticValue(number_match->position), -305.772);

    auto keyword_match = KEYWORD.Match("given f(x) := x^2;");
    ASSERT_TRUE(keyword_match);
    ASSERT_EQ(keyword_match->type, TerminalType::KEYWORD);
    ASSERT_EQ(KEYWORD.SemanticValue(keyword_match->position), "given");
}

TEST(Lex, BasicLexxing)
{
    std::string input_string = "plot plot given -312.4";

    lex.SetInput(input_string);

    auto next = lex.Next();
    ASSERT_EQ(next->type, TerminalType::KEYWORD);

    next = lex.Next();
    ASSERT_EQ(next->type, TerminalType::KEYWORD);

    next = lex.Next();
    ASSERT_EQ(next->type, TerminalType::KEYWORD);

    next = lex.Next();
    ASSERT_EQ(next->type, TerminalType::NUMBER);
}

TEST(Parser, BasicCalculation)
{
    std::string input_string = "calculate 3 + 3";

    lex.SetInput(input_string);

    auto result = program.Parse(lex);

    ASSERT_EQ(result, 6.0);
}