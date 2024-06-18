//
// Created by nathan on 6/8/24.
//

#include "Compiler.h"

using namespace unlogic;

Library unlogic::stdlib("stdlib", {
    {"pow", {
        .function = (void*)(double(*)(double,double))&std::pow,
        .nargs = 2,
    }},
    {"sin", {
        .function = (void*)(double(*)(double))&std::sin,
        .nargs = 1,
    }},
    {"cos", {
        .function = (void*)(double(*)(double))&std::cos,
        .nargs = 1,
    }},
    {"tan", {
        .function = (void*)(double(*)(double))&std::tan,
        .nargs = 1,
    }},
});