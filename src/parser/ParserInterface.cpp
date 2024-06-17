#include "ParserInterface.h"
#include "parser/Scanner.h"
#include "parser/Parser.hpp"

unlogic::Prototype unlogic::parse_prototype(const std::string &input)
{
    Prototype prototype;

    Scanner scanner(input);
    Parser parser(&prototype, scanner);

    parser.parse();

    return prototype;
}