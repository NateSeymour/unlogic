//
// Created by nathan on 6/8/24.
//

#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <expected>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <variant>
#include "Error.h"
#include "Library.h"
#include "graphic/Scene.h"
#include "parser/Parser.h"

namespace unlogic
{
    class Program
    {
        friend class Compiler;

        std::unique_ptr<llvm::orc::LLJIT> jit_;

    protected:
        explicit Program(std::unique_ptr<llvm::orc::LLJIT> jit) : jit_(std::move(jit)) {}

    public:
        void operator()(Scene *scene);

        Program() = delete;
    };

    using CompilationError = std::variant<Error, bf::Error, llvm::Error>;

    class Compiler
    {
        std::vector<Library *> default_libraries_;
        bf::SLRParser<ParserGrammarType> parser_;

    public:
        static void InitializeGlobalCompilerRuntime();

        std::expected<Program, CompilationError> Compile(std::string_view program_text);

        Compiler() = delete;
        Compiler(std::vector<Library *> default_libraries = {});
    };
} // namespace unlogic

#endif // UNLOGIC_COMPILER_H
