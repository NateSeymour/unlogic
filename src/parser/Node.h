#ifndef UNLOGIC_NODE_H
#define UNLOGIC_NODE_H

#include <map>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace unlogic
{
    using Node = std::variant<
            std::monostate,
            struct NumericLiteralNode,
            struct StringLiteralNode,
            struct VariableNode,
            struct CallNode,
            struct AdditionNode,
            struct SubtractionNode,
            struct MultiplicationNode,
            struct DivisionNode,
            struct PotentiationNode,
            struct FunctionDefinitionNode,
            struct PlotCommandNode,
            struct ScopedBlockNode,
            struct ProgramEntryNode
    >;

    using UniqueNode = std::unique_ptr<Node>;

    template<typename T>
    struct Literal
    {
        T value;

        Literal(T value) : value(std::move(value)) {}
        virtual ~Literal() = default;
    };

    struct NumericLiteralNode : Literal<double>
    {
        NumericLiteralNode(double value);
        ~NumericLiteralNode();
    };

    struct StringLiteralNode : Literal<std::string>
    {
        StringLiteralNode(std::string value);
        ~StringLiteralNode();
    };

    struct VariableNode
    {
        std::string identifier;

        VariableNode(std::string identifier);
        ~VariableNode();
    };

    struct CallNode
    {
        std::string function_name;
        std::vector<UniqueNode> arguments;

        CallNode(std::string function_name, std::vector<UniqueNode> arguments);
        ~CallNode();
    };

    struct BinaryNode
    {
        UniqueNode lhs, rhs;

        BinaryNode(UniqueNode lhs, UniqueNode rhs);
        ~BinaryNode();
    };

    struct AdditionNode : BinaryNode
    {
        AdditionNode(UniqueNode lhs, UniqueNode rhs);
        ~AdditionNode();
    };

    struct SubtractionNode : BinaryNode
    {
        SubtractionNode(UniqueNode lhs, UniqueNode rhs);
        ~SubtractionNode();
    };

    struct MultiplicationNode : BinaryNode
    {
        MultiplicationNode(UniqueNode lhs, UniqueNode rhs);
        ~MultiplicationNode();
    };

    struct DivisionNode : BinaryNode
    {
        DivisionNode(UniqueNode lhs, UniqueNode rhs);
        ~DivisionNode();
    };

    struct PotentiationNode : BinaryNode
    {
        PotentiationNode(UniqueNode lhs, UniqueNode rhs);
        ~PotentiationNode();
    };

    struct FunctionDefinitionNode
    {
        std::string name;
        std::vector<std::string> args;
        UniqueNode body;

        FunctionDefinitionNode(std::string name, std::vector<std::string> arguments, UniqueNode body);
        FunctionDefinitionNode(std::string name, UniqueNode body);
        ~FunctionDefinitionNode();
    };

    struct PlotCommandNode
    {
        std::string function_name;

        PlotCommandNode(std::string function_name);
        ~PlotCommandNode();
    };

    struct ScopedBlockNode
    {
        std::vector<UniqueNode> statements;

        ScopedBlockNode(std::vector<UniqueNode> statements);
        ~ScopedBlockNode();
    };

    struct ProgramEntryNode
    {
        UniqueNode body;

        ProgramEntryNode(UniqueNode body);
        ~ProgramEntryNode();
    };

    template<typename T, typename ...Args>
    UniqueNode unique_node(Args &&... args)
    {
        auto temp = std::make_unique<Node>();
        temp->emplace<T>(std::forward<Args>(args)...);
        return std::move(temp);
    }
} // namespace unlogic

#endif // UNLOGIC_NODE_H
