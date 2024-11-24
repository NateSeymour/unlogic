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

LibrarySymbol std_pow(stdlib, "unlogic_std_pow", (void*)unlogic_std_pow, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array args = {
        llvm::Type::getDoubleTy(ctx),
        llvm::Type::getDoubleTy(ctx),
    };
    llvm::Type *ret = llvm::Type::getDoubleTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "unlogic_std_pow", mod);
});

LibrarySymbol std_log(stdlib, "unlogic_std_log", (void*)unlogic_std_log, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array<llvm::Type*, 1> args = {
        llvm::PointerType::getInt8Ty(ctx),
    };
    llvm::Type *ret = llvm::Type::getVoidTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "unlogic_std_log", mod);
});

/*
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, pow, std::pow, UNLOGIC_ARGS(double, double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sin, std::sin, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, cos, std::cos, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, tan, std::tan, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, sqrt, std::sqrt, UNLOGIC_ARGS(double));
UNLOGIC_DEFINE_LIBFUNCTION(stdlib, log, std::log, UNLOGIC_ARGS(double));
*/