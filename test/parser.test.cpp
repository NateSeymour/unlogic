#include <gtest/gtest.h>
#include "parser/Lex.h"
#include "parser/Parser.h"
#include "parser/Node.h"

enum class TerminalType
{
    NUMBER,
    KEYWORD,
};

unlogic::Lex<TerminalType> lex;

unlogic::Terminal<TerminalType, TerminalType::NUMBER, R"(\-?\d*\.\d*)", double> NUMBER(lex, [](auto position) {
    return std::stod(std::string(position.raw));
});

unlogic::Terminal<TerminalType, TerminalType::KEYWORD, R"(given|plot|calculate|for|on)", std::string_view> KEYWORD(lex, [](auto pos) {
    return pos.raw;
});

unlogic::NonTerminal<std::unique_ptr<unlogic::Node>> expression
        = NUMBER
        | KEYWORD
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