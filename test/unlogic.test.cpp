#include <gtest/gtest.h>
#include "parser/ParserInterface.h"
#include "Compiler.h"

TEST(UnlogicParser, NamedFunctionDefinition)
{
    auto prototype = unlogic::parse_prototype("f(x) := 3*x + 2");

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
    auto prototype = unlogic::parse_prototype("f(x, y, z) := 3*x + 2");

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
    auto prototype = unlogic::parse_prototype("3*x + 2 - y");

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
    auto prototype = unlogic::parse_prototype("3*x + 2 - y / z");

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
    auto f = compiler.CompileFunction<double>("f(x) := x");

    ASSERT_EQ(f(2), 2);
}

TEST(UnlogicCompiler, CompileAnonymousFunction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("x");

    ASSERT_EQ(f(2), 2);
}

TEST(UnlogicCompiler, MultipleFunctionDefinitions)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x / 2");
    auto g = compiler.CompileFunction<double>("g(x) := x * 2");
    auto q = compiler.CompileFunction<double>("q(x) := x + 2");

    ASSERT_EQ(f(25), 12.5);
    ASSERT_EQ(g(25), 50);
    ASSERT_EQ(q(25), 27);
}

TEST(Unlogic, MultipleParameterFunction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double, double>("f(x, y) := x * y");

    ASSERT_EQ(f(2, 2), 4);
}

TEST(Unlogic, Addition)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x + 7");

    ASSERT_EQ(f(2), 9);
}

TEST(Unlogic, Subtraction)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x - 7");

    ASSERT_EQ(f(2), -5);
}

TEST(Unlogic, Multiplication)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x * 7");

    ASSERT_EQ(f(2), 14);
}

TEST(Unlogic, Division)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x / 2");

    ASSERT_EQ(f(25), 12.5);
}

TEST(Unlogic, Exponent)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x^2");

    ASSERT_EQ(f(5), 25);
}

TEST(Unlogic, Parentheses)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := x^(2+1)");

    ASSERT_EQ(f(5), 125);
}

TEST(Unlogic, OperatorPrecedence)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<>("3 + 2 * 4");

    ASSERT_EQ(f(), 11);
}

TEST(Unlogic, NegativeNumbers)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<>("-3 + 2 * 4");

    ASSERT_EQ(f(), 5);
}

TEST(Unlogic, StdlibPow)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<double>("f(x) := pow(x, 2)");

    ASSERT_EQ(f(10), 100);
}

TEST(Unlogic, StdlibSin)
{
    unlogic::Compiler compiler;
    auto f = compiler.CompileFunction<>("sin(100)");

    ASSERT_EQ(f(), std::sin(100));
}