//
// Created by nathan on 6/8/24.
//

#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/TargetSelect.h>
#include "parser/Node.h"
#include "parser/Parser.h"
#include "Library.h"
#include "transformer/IRGenerator.h"
#include "graphic/Scene.h"

namespace unlogic
{
    class Program
    {
        friend class Compiler;

        std::unique_ptr<llvm::orc::LLJIT> jit_;

    protected:
        explicit Program(std::unique_ptr<llvm::orc::LLJIT> jit) : jit_(std::move(jit)) {}

    public:
        void operator()(Scene *scene)
        {
            auto function_ea = this->jit_->lookup("__entry");
            if(auto e = function_ea.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            function_ea->toPtr<void(Scene*)>()(scene);
        }

        Program() = delete;
    };

    class Compiler
    {
        std::vector<Library*> default_libraries_;

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

            // Create module
            auto module = std::make_unique<llvm::Module>("unlogic", *ctx.get());

            // Create program scope
            Scope program_scope;
            program_scope.PushLayer();

            for(auto library : this->default_libraries_)
            {
                // Create dylib
                auto dylib = jit->createJITDylib(library->name);
                if(auto e = dylib.takeError())
                {
                    throw std::runtime_error(llvm::toString(std::move(e)));
                }

                // Generate symbol map
                llvm::orc::SymbolMap library_symbols;
                for(auto symbol : library->symbols)
                {
                    library_symbols.insert({
                        jit->mangleAndIntern(symbol->name),
                        symbol->symbol,
                    });

                    symbol->PopulateScope(*ctx.get(), *module, program_scope);
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
            auto result = parser->Parse(program_text);
            auto body = std::get<std::unique_ptr<Node>>(std::move(*result));

            // Create IR generation context
            IRGenerationContext ir_ctx = {
                    .llvm_ctx = *ctx.get(),
                    .module = std::move(module),
                    .scope = program_scope,
            };

            // IR Generator
            IRGenerator generator(ir_ctx);

            // Build program
            body->Accept(generator);

            llvm::orc::ThreadSafeModule tsm(std::move(ir_ctx.module), std::move(ctx));
            auto e = jit->addIRModule(std::move(tsm));

            return Program(std::move(jit));
        }

        Compiler() = default;
        Compiler(std::vector<Library*> default_libraries) : default_libraries_(std::move(default_libraries)) {}
    };
}

#endif //UNLOGIC_COMPILER_H
