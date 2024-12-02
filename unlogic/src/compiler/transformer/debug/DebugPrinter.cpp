//
// Created by Nathan on 12/1/2024.
//

#include "DebugPrinter.h"
#include <format>

std::string unlogic::DebugPrinter::operator()(std::monostate &node)
{
    throw std::runtime_error("unexpected empty node");
}

std::string unlogic::DebugPrinter::operator()(NumericLiteralNode &node)
{
    return std::format("{}", node.value);
}

std::string unlogic::DebugPrinter::operator()(StringLiteralNode &node)
{
    return std::format("\"{}\"", node.value);
}

std::string unlogic::DebugPrinter::operator()(DivisionNode &node)
{
    auto lhs = std::visit(*this, *node.lhs);
    auto rhs = std::visit(*this, *node.rhs);

    return std::format("({} / {})", lhs, rhs);
}

std::string unlogic::DebugPrinter::operator()(ScopedBlockNode &node) {}

std::string unlogic::DebugPrinter::operator()(VariableNode &node)
{
    return node.identifier;
}

std::string unlogic::DebugPrinter::operator()(CallNode &node) {}

std::string unlogic::DebugPrinter::operator()(AdditionNode &node) {}
std::string unlogic::DebugPrinter::operator()(SubtractionNode &node) {}
std::string unlogic::DebugPrinter::operator()(MultiplicationNode &node) {}
std::string unlogic::DebugPrinter::operator()(PotentiationNode &node) {}
std::string unlogic::DebugPrinter::operator()(FunctionDefinitionNode &node) {}
std::string unlogic::DebugPrinter::operator()(PlotCommandNode &node) {}
std::string unlogic::DebugPrinter::operator()(ProgramEntryNode &node) {}
