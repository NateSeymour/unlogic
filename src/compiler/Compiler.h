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
#include "Program.h"
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
        static std::atomic<bool> global_init_complete_;
        std::vector<LibraryDefinition> default_libraries_;

    public:
        static void InitializeGlobalCompilerRuntime()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();

            Compiler::global_init_complete_ = true;
            Compiler::global_init_complete_.notify_all();
        }

        Executable Compile(std::string_view program_text)
        {
            Compiler::global_init_complete_.wait(false);

            // Establish context for build
            auto ctx = std::make_unique<llvm::LLVMContext>();

            // Build parser
            auto parser = bf::SLRParser<ParserGrammarType>::Build(unlogic::tokenizer, unlogic::unlogic_program);

            // Create JIT
            auto jit_res = llvm::orc::LLJITBuilder().create();
            auto jit = std::move(*jit_res);

            // Create and link libraries to main dylib
            llvm::orc::JITDylib &main = jit->getMainJITDylib();

            for(auto &library_definition : this->default_libraries_)
            {
                Library library = library_definition.Build(*ctx.get());

                // Create dylib
                auto dylib = jit->createJITDylib(library.name);
                if(auto e = dylib.takeError())
                {
                    throw std::runtime_error(llvm::toString(std::move(e)));
                }

                // Generate symbol map
                llvm::orc::SymbolMap library_symbols;
                for(auto &function : library.functions)
                {
                    library_symbols.insert({
                        jit->mangleAndIntern(function.function->getName()),
                        function.symbol,
                    });
                }

                // Add symbol map
                auto std_sym_def = dylib->define(llvm::orc::absoluteSymbols(library_symbols));
                if(std_sym_def)
                {
                    throw std::runtime_error(llvm::toString(std::move(std_sym_def)));
                }

                // Add lib
                main.addToLinkOrder(*dylib);
            }

            auto body = std::get<std::unique_ptr<Node>>(*parser->Parse(program_text));

            Program program(*ctx.get(), std::move(body));

            auto module = std::move(program.Build());

            llvm::orc::ThreadSafeModule tsm(std::move(module), std::move(ctx));
            jit->addIRModule(std::move(tsm));

            return Executable(std::move(jit));
        }

        Compiler() = default;
        Compiler(std::vector<LibraryDefinition> default_libraries) : default_libraries_(std::move(default_libraries)) {}
    };
}

#endif //UNLOGIC_COMPILER_H
