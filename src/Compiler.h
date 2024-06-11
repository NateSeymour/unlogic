//
// Created by nathan on 6/8/24.
//

#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <string>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/TargetSelect.h>
#include "Parser.h"

namespace unlogic
{
    template<typename... Args>
    class Callable
    {
        friend class Compiler;
        double(*function_)(Args...) = nullptr;

    protected:
        Callable(llvm::orc::ExecutorAddr function)
        {
            this->function_ = function.toPtr<double(*)(Args...)>();
        }

    public:
        double operator()(Args... args)
        {
            return this->function_(args...);
        }

        Callable() = delete;
    };

    class Compiler
    {
        std::unique_ptr<llvm::orc::LLJIT> jit_;

    public:
        template<typename... Args>
        Callable<Args...> CompileFunction(std::string const &input)
        {
            Parser parser(input);

            CompilationContext ctx;
            Prototype protoype = parser.ParseFunctionDefinition();

            protoype.Codegen(ctx);

            llvm::orc::JITDylib &main = this->jit_->getMainJITDylib();
            auto rt = main.createResourceTracker();

            llvm::orc::ThreadSafeModule tsm(std::move(ctx.module), std::move(ctx.llvm_ctx));
            this->jit_->addIRModule(rt, std::move(tsm));

            auto function = this->jit_->lookup(protoype.name);
            if(auto e = function.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            return Callable<Args...>(*function);
        }

        Compiler()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();

            auto jit = llvm::orc::LLJITBuilder().create();
            if(auto e = jit.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }
            this->jit_ = std::move(*jit);

            auto stdlib = this->jit_->createJITDylib("std");
            if(auto e = stdlib.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }
        }
    };
}

#endif //UNLOGIC_COMPILER_H
