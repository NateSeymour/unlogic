#include <gtest/gtest.h>
#include <cmath>
#include "Program.h"

TEST(Unlogic, Basic)
{
    auto f = unlogic::compile("f[x] = 32*x;");

    double result = f({10.0});

    ASSERT_EQ(result, 320.0);
}

TEST(Unlogic, StressTest)
{
    auto f = unlogic::compile("f[x] = x^2 + 32*x;");

    for(std::size_t i = 0; i < 1000000; i++)
    {
        double result = f({(double)i});

        ASSERT_EQ(result, std::pow(i, 2) + 32 * i);
    }
}