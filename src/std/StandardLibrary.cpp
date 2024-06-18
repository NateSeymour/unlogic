//
// Created by nathan on 6/18/24.
//

#include <cmath>
#include "StandardLibrary.h"

using namespace unlogic;

Library unlogic::stdlib("stdlib");

UNLOGIC_DEFINE_LIBFUNCTION(stdlib, pow, std::pow, UNLOGIC_ARGS(double, double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sin, std::sin, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, cos, std::cos, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, tan, std::tan, UNLOGIC_ARGS(double));
