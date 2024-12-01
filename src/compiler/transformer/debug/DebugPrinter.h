//
// Created by Nathan on 12/1/2024.
//

#ifndef DEBUGPRINTER_H
#define DEBUGPRINTER_H

#include <string>
#include "parser/Node.h"

namespace unlogic
{
    struct DebugPrinter
    {
        std::string operator()(std::monostate &node);
        std::string operator()(NumericLiteralNode &node);
        std::string operator()(StringLiteralNode &node);
        std::string operator()(DivisionNode &node);
        std::string operator()(ScopedBlockNode &node);
        std::string operator()(VariableNode &node);
        std::string operator()(CallNode &node);
        std::string operator()(AdditionNode &node);
        std::string operator()(SubtractionNode &node);
        std::string operator()(MultiplicationNode &node);
        std::string operator()(PotentiationNode &node);
        std::string operator()(FunctionDefinitionNode &node);
        std::string operator()(PlotCommandNode &node);
        std::string operator()(ProgramEntryNode &node);
    };
} // namespace unlogic

#endif // DEBUGPRINTER_H
