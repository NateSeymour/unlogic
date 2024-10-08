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
#include "parser/Parser.h"
#include "Library.h"
#include "std/StandardLibrary.h"

namespace unlogic
{
    class Executable
    {
        friend class Compiler;

        void(*function_)() = nullptr;
        std::unique_ptr<llvm::orc::LLJIT> jit_;

    protected:
        explicit Executable(std::unique_ptr<llvm::orc::LLJIT> jit) : jit_(std::move(jit))
        {
            auto function_ea = this->jit_->lookup("__entry");
            if(auto e = function_ea.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            this->function_ = function_ea->toPtr<void(*)()>();
        }

    public:
        void operator()() const
        {
            return this->function_();
        }

        Executable() = delete;
    };

    class Compiler
    {
        static std::vector<Library> global_libraries_;

    public:
        static void InitializeCompilerRuntime()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
        }

        static Executable CompileProgram(Program &program)
        {
            auto jit_res = llvm::orc::LLJITBuilder().create();
            auto jit = std::move(*jit_res);

            CompilationContext ctx;

            llvm::orc::JITDylib &main = jit->getMainJITDylib();

            // Add libraries
            for(auto &library : Compiler::global_libraries_)
            {
                // Create dylib
                auto dylib = jit->createJITDylib(library.name_);
                if(auto e = dylib.takeError())
                {
                    throw std::runtime_error(llvm::toString(std::move(e)));
                }

                // Instantiate lib
                auto symbol_map = library.SymbolMap(*jit);
                auto std_sym_def = dylib->define(llvm::orc::absoluteSymbols(symbol_map));
                if(std_sym_def)
                {
                    throw std::runtime_error(llvm::toString(std::move(std_sym_def)));
                }

                // Add lib
                main.addToLinkOrder(*dylib);

                // Add to compilation context
                library.PopulateCompilationContext(ctx);
            }

            program.Codegen(ctx);

            auto rt = main.createResourceTracker();

            llvm::orc::ThreadSafeModule tsm(std::move(ctx.module), std::move(ctx.llvm_ctx));
            jit->addIRModule(rt, std::move(tsm));

            return Executable(std::move(jit));
        }

        static void RegisterGlobalLibrary(Library const &library)
        {
            Compiler::global_libraries_.push_back(library);
        }

        Compiler() = default;
    };
}

#endif //UNLOGIC_COMPILER_H
