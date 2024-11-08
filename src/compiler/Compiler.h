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
#include "transformer/IRGenerator.h"

namespace unlogic
{
    class Program
    {
        friend class Compiler;

        std::unique_ptr<llvm::orc::LLJIT> jit_;

    protected:
        explicit Program(std::unique_ptr<llvm::orc::LLJIT> jit) : jit_(std::move(jit))
        {
            auto function_ea = this->jit_->lookup("__entry");
            if(auto e = function_ea.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            this->function_ = function_ea->toPtr<void(*)()>();
        }

    public:


        Program() = delete;
    };

    class Compiler
    {
        std::vector<LibraryDefinition> default_libraries_;

    public:
        static void InitializeGlobalCompilerRuntime()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
        }

        Program Compile(std::string_view program_text)
        {
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

            // Parse program
            auto body = std::get<std::unique_ptr<Node>>(*parser->Parse(program_text));

            // Compile program
            auto module = std::make_unique<llvm::Module>("unlogic", *ctx.get());

            // Create IR generation context
            Scope program_scope;
            IRGenerationContext ir_ctx = {
                    .llvm_ctx = *ctx.get(),
                    .module = std::move(module),
                    .scope = program_scope,
            };

            // IR Generator
            IRGenerator generator(ir_ctx);

            // Build program
            body->Accept(generator);

            llvm::orc::ThreadSafeModule tsm(std::move(module), std::move(ctx));
            jit->addIRModule(std::move(tsm));

            return Program(std::move(jit));
        }

        Compiler() = default;
        Compiler(std::vector<LibraryDefinition> default_libraries) : default_libraries_(std::move(default_libraries)) {}
    };
}

#endif //UNLOGIC_COMPILER_H
