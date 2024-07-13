#include "parser/Lex.h"

enum class TerminalType
{
    NUMBER,
};

unlogic::Lex<TerminalType> lex;

unlogic::Terminal<TerminalType, TerminalType::NUMBER, "\-?\d*\.\d*", double> number(lex, [](auto position) {
    return 0.0;
});