#ifndef UNLOGIC_COMPILER_H
#define UNLOGIC_COMPILER_H

#include <city/Assembly.h>
#include <city/JIT.h>
#include <expected>
#include <variant>
#include "Error.h"
#include "Scope.h"
#include "parser/Node.h"
#include "parser/Parser.h"

namespace unlogic
{
    using CompilationError = std::variant<Error, bf::Error>;

    class Compiler
    {
        city::JIT jit_;
        Scope scope_;
        bf::SLRParser<ParserGrammarType> parser_;

    public:
        std::expected<city::Assembly, CompilationError> Compile(std::string_view program_text, std::vector<bf::Token<ParserGrammarType>> *tokens = nullptr);

        Compiler();
    };
} // namespace unlogic

#endif // UNLOGIC_COMPILER_H
