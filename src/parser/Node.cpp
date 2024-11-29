/**
 * @file Declarations for ctors and dtors of all node types in order to allow them to have unique pointers to the "Node" alias type.
 */
#include "Node.h"

using namespace unlogic;

NumericLiteralNode::NumericLiteralNode(double value) : Literal(value) {}
NumericLiteralNode::~NumericLiteralNode() = default;

StringLiteralNode::StringLiteralNode(std::string value) : Literal(std::move(value)) {}
StringLiteralNode::~StringLiteralNode() = default;

VariableNode::VariableNode(std::string identifier) : identifier(std::move(identifier)) {}
VariableNode::~VariableNode() = default;

CallNode::CallNode(std::string function_name, std::vector<UniqueNode> arguments) : function_name(std::move(function_name)), arguments(std::move(arguments)) {}
CallNode::~CallNode() = default;

BinaryNode::BinaryNode(UniqueNode lhs, UniqueNode rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
BinaryNode::~BinaryNode() = default;

AdditionNode::AdditionNode(UniqueNode lhs, UniqueNode rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
AdditionNode::~AdditionNode() = default;

SubtractionNode::SubtractionNode(UniqueNode lhs, UniqueNode rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
SubtractionNode::~SubtractionNode() = default;

MultiplicationNode::MultiplicationNode(UniqueNode lhs, UniqueNode rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
MultiplicationNode::~MultiplicationNode() = default;

DivisionNode::DivisionNode(UniqueNode lhs, UniqueNode rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
DivisionNode::~DivisionNode() = default;

PotentiationNode::PotentiationNode(UniqueNode lhs, UniqueNode rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
PotentiationNode::~PotentiationNode() = default;

FunctionDefinitionNode::FunctionDefinitionNode(std::string name, std::vector<std::string> arguments, UniqueNode body) : name(std::move(name)), args(std::move(arguments)), body(std::move(body)) {}
FunctionDefinitionNode::FunctionDefinitionNode(std::string name, UniqueNode body) : name(std::move(name)), body(std::move(body)) {}
FunctionDefinitionNode::~FunctionDefinitionNode() = default;

PlotCommandNode::PlotCommandNode(std::string function_name) : function_name(std::move(function_name)) {}
PlotCommandNode::~PlotCommandNode() = default;

ScopedBlockNode::ScopedBlockNode(std::vector<UniqueNode> statements) : statements(std::move(statements)) {}
ScopedBlockNode::~ScopedBlockNode() = default;

ProgramEntryNode::ProgramEntryNode(UniqueNode body) : body(std::move(body)) {}
ProgramEntryNode::~ProgramEntryNode() = default;
