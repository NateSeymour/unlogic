#include <iostream>
#include "MathFunction.h"

int main()
{
    EvaluationContext context = {
            .parameters = {},
    };
    MathFunction eq("f[x, y] = x * y; f(bigboy, f(2, 2));");

    double value = eq.Evaluate(context);

    std::cout << value << std::endl;

    return 0;
}
