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

LibraryDefinition unlogic::stdlib("stdlib", [](unlogic::Library &lib) {
    // pow
    std::array<llvm::Type*, 2> pow_args = {
            llvm::Type::getDoubleTy(lib.ctx),
            llvm::Type::getDoubleTy(lib.ctx),
    };

    llvm::FunctionType *pow_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(lib.ctx), pow_args, false);
    lib.AddFunction("pow", pow_type, (void *) unlogic_std_pow);

    // log
    std::array<llvm::Type*, 1> log_args = {
            llvm::Type::getInt8PtrTy(lib.ctx),
    };

    llvm::FunctionType *log_type = llvm::FunctionType::get(llvm::Type::getVoidTy(lib.ctx), log_args, false);
    lib.AddFunction("log", log_type, (void *) unlogic_std_log);
});

/*
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, pow, std::pow, UNLOGIC_ARGS(double, double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sin, std::sin, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, cos, std::cos, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, tan, std::tan, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sqrt, std::sqrt, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, log, std::log, UNLOGIC_ARGS(double));
*/