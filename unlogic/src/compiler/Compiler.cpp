#include "Compiler.h"
#include "parser/Node.h"
#include "transformer/IRGenerator.h"

using namespace unlogic;

void Program::operator()(Scene *scene)
{
    auto function = this->assembly_["__entry"];

    auto program = function.ToPointer<void(Scene *)>();

    return program(scene);
}

std::expected<Program, CompilationError> Compiler::Compile(std::string_view program_text, std::vector<bf::Token<ParserGrammarType>> *tokens)
{
    auto ast = this->parser_.Parse(program_text, tokens);
    if (!ast.has_value())
    {
        return std::unexpected(ast.error());
    }
    auto ast_body = std::get<std::unique_ptr<Node>>(std::move(*ast));

    city::IRModule module{"unlogic_user_program"};

    Scope program_scope;
    program_scope.PushLayer();

    IRGenerationContext ir_ctx = {
            .module = module,
            .scope = program_scope,
    };

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

    this->jit_.InsertIRModule(std::move(module));
    return Program(this->jit_.CompileAndLink());
}

Compiler::Compiler() : parser_(*bf::SLRParser<ParserGrammarType>::Build(unlogic_program))
{
    // TODO: Add standard library modules
}
