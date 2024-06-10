#include <gtest/gtest.h>
#include "Parser.h"
#include "Compiler.h"

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

TEST(UnlogicParser, NamedFunctionMultipleParameterDefinition)
{
    unlogic::Parser parser("f(x, y, z) = 3*x + 2");

    auto prototype = parser.ParseFunctionDefinition();

    ASSERT_EQ(prototype.name, "f");
    ASSERT_EQ(prototype.anonymous, false);

    std::vector<std::string> expected_arguments = {"x", "y", "z"};
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

TEST(UnlogicParser, AnonymousFunctionMultipleParameterDefinition)
{
    unlogic::Parser parser("3*x + 2 - y / z");

    auto prototype = parser.ParseFunctionDefinition();

    ASSERT_EQ(prototype.name, "__anon");
    ASSERT_EQ(prototype.anonymous, true);

    std::vector<std::string> expected_arguments = {"x", "y", "z"};
    for(auto const &[actual, expected] : std::ranges::zip_view(prototype.arguments, expected_arguments))
    {
        ASSERT_EQ(actual, expected);
    }
}

TEST(UnlogicCompiler, CompileNamedFunction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) = x");

    ASSERT_EQ(f(2), 2);
}

TEST(UnlogicCompiler, CompileAnonymousFunction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("x");

    ASSERT_EQ(f(2), 2);
}

TEST(Unlogic, MultipleParameterFunction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double, double>("f(x, y) = x * y");

    ASSERT_EQ(f(2, 2), 4);
}

TEST(Unlogic, Addition)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) = x + 7");

    ASSERT_EQ(f(2), 9);
}

TEST(Unlogic, Subtraction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) = x - 7");

    ASSERT_EQ(f(2), -5);
}

TEST(Unlogic, Multiplication)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) = x * 7");

    ASSERT_EQ(f(2), 14);
}

TEST(Unlogic, Division)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) = x / 2");

    ASSERT_EQ(f(25), 12.5);
}