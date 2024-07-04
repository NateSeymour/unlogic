//
// Created by nathan on 6/8/24.
//

#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <cmath>
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
#include "parser/Node.h"
#include "parser/Parser.hpp"
#include "parser/Scanner.h"
#include "Library.h"
#include "std/StandardLibrary.h"

namespace unlogic
{
    template<typename... Args>
    class Callable
    {
        friend class Compiler;
        double(*function_)(Args...) = nullptr;

    protected:
        explicit Callable(llvm::orc::ExecutorAddr function)
        {
            this->function_ = function.toPtr<double(*)(Args...)>();
        }

    public:
        double operator()(Args... args) const
        {
            return this->function_(args...);
        }

        Callable() = delete;
    };

    class Compiler
    {
        std::unique_ptr<llvm::orc::LLJIT> jit_;
        std::vector<Library> libraries_;

    public:
        static void InitializeCompilerRuntime()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
        }

        template<typename... Args>
        Callable<Args...> CompileFunction(std::string const &input)
        {
            CompilationContext ctx;
            for(auto const &library : this->libraries_)
            {
                library.PopulateCompilationContext(ctx);
            }

            Prototype prototype = unlogic::parse_prototype(input);

            prototype.Codegen(ctx);

            llvm::orc::JITDylib &main = this->jit_->getMainJITDylib();
            auto rt = main.createResourceTracker();

            llvm::orc::ThreadSafeModule tsm(std::move(ctx.module), std::move(ctx.llvm_ctx));
            this->jit_->addIRModule(rt, std::move(tsm));

            auto function = this->jit_->lookup(prototype.name);
            if(auto e = function.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            return Callable<Args...>(*function);
        }

        void AddLibrary(Library const &library)
        {
            auto dylib = this->jit_->createJITDylib(library.name_);
            if(auto e = dylib.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            // Instantiate stdlib
            auto symbol_map = library.SymbolMap(*this->jit_);
            auto std_sym_def = dylib->define(llvm::orc::absoluteSymbols(symbol_map));
            if(std_sym_def)
            {
                throw std::runtime_error(llvm::toString(std::move(std_sym_def)));
            }

            // Add stdlib to link order
            llvm::orc::JITDylib &main = this->jit_->getMainJITDylib();
            main.addToLinkOrder(*dylib);

            this->libraries_.push_back(library);
        }

        Compiler()
        {
            auto jit = llvm::orc::LLJITBuilder().create();
            if(auto e = jit.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }
            this->jit_ = std::move(*jit);

            this->AddLibrary(stdlib);
        }
    };
}

#endif //UNLOGIC_COMPILER_H
