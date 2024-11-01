//
// Created by nathan on 6/18/24.
//

#include <cmath>
#include "StandardLibrary.h"

using namespace unlogic;

extern "C"
{
    double unlogic_pow(double base, double exponent) { return std::pow(base, exponent); }
}

LibraryDefinition unlogic::stdlib("stdlib", [](unlogic::Library &lib) {
    std::array<llvm::Type*, 2> pow_args = { llvm::Type::getDoubleTy(lib.ctx), llvm::Type::getDoubleTy(lib.ctx) };
    llvm::FunctionType *pow_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(lib.ctx), pow_args, false);
    lib.AddFunction("pow", pow_type, (void*)unlogic_pow);
});

/*
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, pow, std::pow, UNLOGIC_ARGS(double, double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sin, std::sin, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, cos, std::cos, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, tan, std::tan, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sqrt, std::sqrt, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, log, std::log, UNLOGIC_ARGS(double));
*/