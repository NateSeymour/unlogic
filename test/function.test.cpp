#include <gtest/gtest.h>
#include <cmath>
#include "Parser.h"

TEST(Unlogic, Basic)
{
    Math m("f(x) = 3x + 4;");
    Callable f = unlogic::compile("f(x) = 3x + 4;");

    //
    EvaluationContext ctx;
    Parser f("f[x] = 32*x; f(x);");

    ctx.parameters["x"] = 10;

    double result = f.Evaluate(ctx);

    ASSERT_EQ(result, 320.0);
}

TEST(Unlogic, StressTest)
{
    EvaluationContext ctx;
    Parser f("f[x] = x^2 + 32*x; f(x);");

    for(std::size_t i = 0; i < 1000000; i++)
    {
        ctx.parameters["x"] = i;

        double result = f.Evaluate(ctx);

        ASSERT_EQ(result, std::pow(i, 2) + 32 * i);
    }
}