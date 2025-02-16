#include <format>
#include <gtest/gtest.h>
#include <random>
#include "compiler/Compiler.h"

class UnlogicTestRunner : public testing::Test
{
protected:
    template<typename T>
    [[nodiscard]] std::pair<T, T> GetRandomPair() const
    {
        return {
                static_cast<T>(std::rand()) + (static_cast<T>(std::rand()) / RAND_MAX),
                static_cast<T>(std::rand()) + (static_cast<T>(std::rand()) / RAND_MAX),
        };
    }
    void SetUp() override
    {
        std::srand(std::time({}));
    }

    void TearDown() override {}

public:
    static constexpr int TEST_ROUNDS = 1000;
};

TEST_F(UnlogicTestRunner, ConstantAddition)
{
    unlogic::Compiler compiler;

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        auto program = std::format("given f(x) = x + {};", values.second);
        auto assembly = *compiler.Compile(program);

        auto f = assembly["f"].ToPointer<double(double)>();
        EXPECT_EQ(f(values.first), values.first + values.second);
    }
}

TEST_F(UnlogicTestRunner, ConstantSubtraction)
{
    unlogic::Compiler compiler;

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        auto program = std::format("given f(x) = x - {};", values.second);
        auto assembly = *compiler.Compile(program);

        auto f = assembly["f"].ToPointer<double(double)>();
        EXPECT_EQ(f(values.first), values.first - values.second);
    }
}

TEST_F(UnlogicTestRunner, ConstantMultiplication)
{
    unlogic::Compiler compiler;

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        auto program = std::format("given f(x) = x * {};", values.second);
        auto assembly = *compiler.Compile(program);

        auto f = assembly["f"].ToPointer<double(double)>();
        EXPECT_EQ(f(values.first), values.first * values.second);
    }
}

TEST_F(UnlogicTestRunner, ConstantDivision)
{
    unlogic::Compiler compiler;

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        auto program = std::format("given f(x) = x / {};", values.second);
        auto assembly = *compiler.Compile(program);

        auto f = assembly["f"].ToPointer<double(double)>();
        EXPECT_EQ(f(values.first), values.first / values.second);
    }
}

TEST_F(UnlogicTestRunner, Addition)
{
    unlogic::Compiler compiler;

    auto assembly = *compiler.Compile("given f(x, y) = x + y;");

    auto f = assembly["f"].ToPointer<double(double, double)>();

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        EXPECT_EQ(f(values.first, values.second), values.first + values.second);
    }
}

TEST_F(UnlogicTestRunner, Subtraction)
{
    unlogic::Compiler compiler;

    auto assembly = *compiler.Compile("given f(x, y) = x - y;");

    auto f = assembly["f"].ToPointer<double(double, double)>();

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        EXPECT_EQ(f(values.first, values.second), values.first - values.second);
    }
}

TEST_F(UnlogicTestRunner, Multiplication)
{
    unlogic::Compiler compiler;

    auto assembly = *compiler.Compile("given f(x, y) = x * y;");

    auto f = assembly["f"].ToPointer<double(double, double)>();

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        EXPECT_EQ(f(values.first, values.second), values.first * values.second);
    }
}

TEST_F(UnlogicTestRunner, Division)
{
    unlogic::Compiler compiler;

    auto assembly = *compiler.Compile("given f(x, y) = x / y;");

    auto f = assembly["f"].ToPointer<double(double, double)>();

    for (int i = 0; i < TEST_ROUNDS; i++)
    {
        auto values = this->GetRandomPair<double>();

        EXPECT_EQ(f(values.first, values.second), values.first / values.second);
    }
}
