//
// Created by nathan on 6/8/24.
//

#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <variant>
#include <expected>
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
#include "Error.h"

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
            if (auto e = function_ea.takeError())
            {
                throw std::runtime_error(llvm::toString(std::move(e)));
            }

            auto program = function_ea->toPtr<void(Scene *)>();

            return program(scene);
        }

        Program() = delete;
    };

    using CompilationError = std::variant<Error, bf::Error, llvm::Error>;

    class Compiler
    {
        std::vector<Library *> default_libraries_;
        bf::SLRParser<ParserGrammarType> parser_;

    public:
        static void InitializeGlobalCompilerRuntime()
        {
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
        }

        std::expected<Program, CompilationError> Compile(std::string_view program_text)
        {
            // Parse program
            auto ast = this->parser_.Parse(program_text);
            if (!ast.has_value())
            {
                return std::unexpected(ast.error());
            }

            auto ast_body = std::get<std::unique_ptr<Node>>(std::move(*ast));

            // Establish context for build
            auto ctx = std::make_unique<llvm::LLVMContext>();

            auto jit = std::move(*llvm::orc::LLJITBuilder().create());

            // Create and link libraries to main dylib
            llvm::orc::JITDylib &main = jit->getMainJITDylib();

            // Create module
            auto module = std::make_unique<llvm::Module>("unlogic", *ctx.get());

            // Create program scope
            Scope program_scope;
            program_scope.PushLayer();

            for (auto library: this->default_libraries_)
            {
                // Generate symbol map
                llvm::orc::SymbolMap library_symbols;
                for (auto symbol: library->symbols)
                {
                    library_symbols.insert({
                            jit->mangleAndIntern(symbol->name),
                            symbol->symbol,
                    });

                    symbol->Define(*ctx.get(), *module);
                }

                // Add symbol map
                auto std_sym_def = main.define(llvm::orc::absoluteSymbols(library_symbols));
                if (std_sym_def)
                {
                    return std::unexpected(std::move(std_sym_def));
                }
            }

            // Create IR generation context
            IRGenerationContext ir_ctx = {
                    .llvm_ctx = *ctx.get(),
                    .module = std::move(module),
                    .scope = program_scope,
            };

            // IR Generator
            IRGenerator generator(ir_ctx);

            // Build program
            try
            {
                ast_body->Accept(generator);
            }
            catch (std::runtime_error &e)
            {
                return std::unexpected(Error{e.what()});
            }

            llvm::orc::ThreadSafeModule tsm(std::move(ir_ctx.module), std::move(ctx));
            auto e = jit->addIRModule(std::move(tsm));

            return Program(std::move(jit));
        }

        Compiler() = delete;
        Compiler(std::vector<Library *> default_libraries = {}) :
            default_libraries_(std::move(default_libraries)), parser_(*bf::SLRParser<ParserGrammarType>::Build(unlogic::tokenizer, unlogic::unlogic_program))
        {
        }
    };
} // namespace unlogic

#endif // UNLOGIC_COMPILER_H
