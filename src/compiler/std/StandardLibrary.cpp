//
// Created by nathan on 6/18/24.
//

#include <iostream>
#include <cmath>
#include "StandardLibrary.h"

using namespace unlogic;

extern "C"
{
    double unlogic_std_pow(double base, double exponent) { return std::pow(base, exponent); }

    void unlogic_std_log(char const *message)
    {
        std::cout << message << std::endl;
    }
}

Library unlogic::stdlib("stdlib");

LibrarySymbol std_pow(stdlib, "pow", (void*) unlogic_std_pow);
LibrarySymbol std_log(stdlib, "log", (void*) unlogic_std_log);

/*
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, pow, std::pow, UNLOGIC_ARGS(double, double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sin, std::sin, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, cos, std::cos, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, tan, std::tan, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sqrt, std::sqrt, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, log, std::log, UNLOGIC_ARGS(double));
*/