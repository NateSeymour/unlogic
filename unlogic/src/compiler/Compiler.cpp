//
// Created by nathan on 6/8/24.
//

#include "Compiler.h"
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/TargetSelect.h>
#include "parser/Node.h"
#include "transformer/IRGenerator.h"

using namespace unlogic;

void Program::operator()(Scene *scene)
{
    auto function_ea = this->jit_->lookup("__entry");
    if (auto e = function_ea.takeError())
    {
        throw std::runtime_error(llvm::toString(std::move(e)));
    }

    auto program = function_ea->toPtr<void(Scene *)>();

    return program(scene);
}

void Compiler::InitializeGlobalCompilerRuntime()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
}

std::expected<Program, CompilationError> Compiler::Compile(std::string_view program_text, std::vector<bf::Token<ParserGrammarType>> *tokens)
{
    // Parse program
    auto ast = this->parser_.Parse(program_text, tokens);
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
        std::visit(generator, *ast_body);
    }
    catch (std::runtime_error &e)
    {
        return std::unexpected(Error{e.what()});
    }

    llvm::orc::ThreadSafeModule tsm(std::move(ir_ctx.module), std::move(ctx));
    auto e = jit->addIRModule(std::move(tsm));

    return Program(std::move(jit));
}

Compiler::Compiler(std::vector<Library *> default_libraries) :
    default_libraries_(std::move(default_libraries)), parser_(*bf::SLRParser<ParserGrammarType>::Build(unlogic::unlogic_program))
{
}
