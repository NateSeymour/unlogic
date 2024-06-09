#include <gtest/gtest.h>
#include "Parser.h"

TEST(UnlogicParser, NamedFunctionDefinition)
{
    unlogic::Parser parser("f(x) = 3*x + 2");

    auto prototype = parser.ParseFunctionDefinition();

    ASSERT_EQ(prototype.name, "f");
    ASSERT_EQ(prototype.anonymous, false);

    std::vector<std::string> expected_arguments = {"x"};
    for(auto const &[actual, expected] : std::ranges::zip_view(prototype.arguments, expected_arguments))
    {
        ASSERT_EQ(actual, expected);
    }
}

TEST(UnlogicParser, AnonymousFunctionDefinition)
{
    unlogic::Parser parser("3*x + 2 - y");

    auto prototype = parser.ParseFunctionDefinition();

    ASSERT_EQ(prototype.name, "__anon");
    ASSERT_EQ(prototype.anonymous, true);

    std::vector<std::string> expected_arguments = {"x", "y"};
    for(auto const &[actual, expected] : std::ranges::zip_view(prototype.arguments, expected_arguments))
    {
        ASSERT_EQ(actual, expected);
    }
}