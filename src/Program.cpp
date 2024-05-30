#include "Program.h"

unlogic::Function unlogic::compile(const std::string &function)
{
    Parser parser(function);

    auto function_definition = parser.ParseFunctionDefinition();

    return function_definition->GenerateFunction();
}
