//
// Created by nathan on 6/18/24.
//

#include <iostream>
#include <cmath>
#include "StandardLibrary.h"

using namespace unlogic;

extern "C"
{
    double unlogic_std_pow(double base, double exponent)
    {
        return std::pow(base, exponent);
    }

    void unlogic_std_log(char const *message)
    {
        std::cout << message << std::endl;
    }

    double unlogic_std_sin(double x)
    {
        return std::sin(x);
    }

    double unlogic_std_cos(double x)
    {
        return std::cos(x);
    }

    double unlogic_std_tan(double x)
    {
        return std::tan(x);
    }

    double unlogic_std_sqrt(double x)
    {
        return std::sqrt(x);
    }
}

Library unlogic::stdlib("stdlib");

LibrarySymbol std_pow(stdlib, "pow", (void *)unlogic_std_pow, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array args = {
            llvm::Type::getDoubleTy(ctx),
            llvm::Type::getDoubleTy(ctx),
    };
    llvm::Type *ret = llvm::Type::getDoubleTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "pow", mod);
});

LibrarySymbol std_log(stdlib, "log", (void *)unlogic_std_log, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array<llvm::Type *, 1> args = {
            llvm::PointerType::getInt8Ty(ctx),
    };
    llvm::Type *ret = llvm::Type::getVoidTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "log", mod);
});

LibrarySymbol std_sin(stdlib, "sin", (void *)unlogic_std_sin, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array<llvm::Type *, 1> args = {
            llvm::Type::getDoubleTy(ctx),
    };
    llvm::Type *ret = llvm::Type::getDoubleTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "sin", mod);
});

LibrarySymbol std_cos(stdlib, "cos", (void *)unlogic_std_cos, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array<llvm::Type *, 1> args = {
            llvm::Type::getDoubleTy(ctx),
    };
    llvm::Type *ret = llvm::Type::getDoubleTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "cos", mod);
});

LibrarySymbol std_tan(stdlib, "tan", (void *)unlogic_std_tan, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array<llvm::Type *, 1> args = {
            llvm::Type::getDoubleTy(ctx),
    };
    llvm::Type *ret = llvm::Type::getDoubleTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "tan", mod);
});

LibrarySymbol std_sqrt(stdlib, "sqrt", (void *)unlogic_std_sqrt, [](llvm::LLVMContext &ctx, llvm::Module &mod) {
    std::array<llvm::Type *, 1> args = {
            llvm::Type::getDoubleTy(ctx),
    };
    llvm::Type *ret = llvm::Type::getDoubleTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "sqrt", mod);
});
