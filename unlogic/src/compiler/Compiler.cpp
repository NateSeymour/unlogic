#include "Compiler.h"
#include <cmath>
#include "graphic/Scene.h"
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

    this->jit_.RemoveModule("__user");

    try
    {
        city::IRModule module{"__user"};

        this->scope_.PushLayer();

        IRGenerationContext ctx = {
                .module = module,
                .scope = this->scope_,
        };

        std::visit(IRGenerator{ctx}, *ast_body);

        this->scope_.PopLayer();

        this->jit_.InsertIRModule(std::move(module));
    }
    catch (std::runtime_error &e)
    {
        return std::unexpected(Error{e.what()});
    }

    return Program(this->jit_.Link());
}

extern "C"
{
    void unlogic_plot_anon(Scene *scene, Plot2dFunctionType function)
    {
        scene->AddPlot("anon", function);
    }
}

Compiler::Compiler() : parser_(*bf::SLRParser<ParserGrammarType>::Build(unlogic_program))
{
    this->scope_.PushLayer();

    city::InterfaceModule stdlib{"std"};
    city::Function *std_functions[] = {
            stdlib.InsertBinding("__pow", (double (*)(double, double))std::pow),
            stdlib.InsertBinding("__sqrt", (double (*)(double))std::sqrt),

            stdlib.InsertBinding("__plot_anon", unlogic_plot_anon),
    };

    for (auto function: std_functions)
    {
        this->scope_.Set(function->GetName(), function);
    }

    this->jit_.InsertInterfaceModule(std::move(stdlib));
}
