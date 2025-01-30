#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <city/Assembly.h>
#include <city/JIT.h>
#include <expected>
#include <variant>
#include "Error.h"
#include "graphic/Scene.h"
#include "parser/Node.h"
#include "parser/Parser.h"

namespace unlogic
{
    class Program
    {
        friend class Compiler;

        city::Assembly assembly_;

    protected:
        explicit Program(city::Assembly &&assembly) : assembly_(std::move(assembly)) {}

    public:
        void operator()(Scene *scene);

        Program() = delete;
    };

    using CompilationError = std::variant<Error, bf::Error>;

    class Compiler
    {
        city::JIT jit_;
        bf::SLRParser<ParserGrammarType> parser_;

    public:
        std::expected<Program, CompilationError> Compile(std::string_view program_text, std::vector<bf::Token<ParserGrammarType>> *tokens = nullptr);

        Compiler();
    };
} // namespace unlogic

#endif // UNLOGIC_COMPILER_H
