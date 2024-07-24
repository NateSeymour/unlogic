#include "parser/Tokenizer.h"

enum class TerminalType
{
    NUMBER,
};

unlogic::Tokenizer<TerminalType> tokenizer;

unlogic::Terminal<TerminalType, TerminalType::NUMBER, "\-?\d*\.\d*", double> number(tokenizer, [](auto position) {
    return 0.0;
});