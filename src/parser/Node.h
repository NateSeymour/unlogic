#ifndef UNLOGIC_NODE_H
#define UNLOGIC_NODE_H

#include <string>
#include <stdexcept>
#include <memory>
#include <map>
#include <ranges>
#include <vector>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

namespace unlogic
{
    struct NumericLiteralNode;
    struct StringLiteralNode;
    struct VariableNode;
    struct CallNode;
    struct AdditionNode;
    struct SubtractionNode;
    struct MultiplicationNode;
    struct DivisionNode;
    struct PotentiationNode;
    struct FunctionDefinitionNode;
    struct ScopedBlockNode;
    struct ProgramEntryNode;

    struct INodeVisitor
    {
        virtual void Visit(NumericLiteralNode const *node)      = 0;
        virtual void Visit(StringLiteralNode const *node)       = 0;
        virtual void Visit(VariableNode const *node)            = 0;
        virtual void Visit(CallNode const *node)                = 0;
        virtual void Visit(AdditionNode const *node)            = 0;
        virtual void Visit(SubtractionNode const *node)         = 0;
        virtual void Visit(MultiplicationNode const *node)      = 0;
        virtual void Visit(DivisionNode const *node)            = 0;
        virtual void Visit(PotentiationNode const *node)        = 0;
        virtual void Visit(FunctionDefinitionNode const *node)  = 0;
        virtual void Visit(ScopedBlockNode const *node)         = 0;
        virtual void Visit(ProgramEntryNode const *node)        = 0;
    };

    struct Node
    {
        virtual void Accept(INodeVisitor &visitor) = 0;
        virtual ~Node() = default;
    };

    template<typename T>
    struct Literal
    {
        T value;

        Literal(T value) : value(std::move(value)) {}
        virtual ~Literal() = default;
    };

    struct NumericLiteralNode : public Node, public Literal<double>
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        NumericLiteralNode(double value) : Literal(value) {}
    };

    struct StringLiteralNode : public Node, public Literal<std::string>
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        StringLiteralNode(std::string value) : Literal(std::move(value)) {}
    };

    struct VariableNode : public Node
    {
        std::string identifier_;

        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        VariableNode(std::string identifier) : identifier_(std::move(identifier)) {}
    };

    struct CallNode : public Node
    {
        std::string function_name_;
        std::vector<std::unique_ptr<Node>> arguments_;

        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        CallNode(std::string function_name, std::vector<std::unique_ptr<Node>> arguments) : function_name_(std::move(function_name)), arguments_(std::move(arguments)) {}
    };

    struct BinaryNode : public Node
    {
        std::unique_ptr<Node> lhs_, rhs_;

        BinaryNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    };

    struct AdditionNode : public BinaryNode
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        AdditionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    struct SubtractionNode : public BinaryNode
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        SubtractionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    struct MultiplicationNode : public BinaryNode
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        MultiplicationNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    struct DivisionNode : public BinaryNode
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        DivisionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    struct PotentiationNode : public BinaryNode
    {
        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        PotentiationNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    struct FunctionDefinitionNode : public Node
    {
        std::string name_;
        std::vector<std::string> args_;
        std::unique_ptr<Node> body_;

        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        FunctionDefinitionNode(std::string name, std::vector<std::string> arguments, std::unique_ptr<Node> body)
            : name_(std::move(name)),
              args_(std::move(arguments)),
              body_(std::move(body)) {}

        FunctionDefinitionNode(std::string name, std::unique_ptr<Node> body)
            : name_(std::move(name)),
              body_(std::move(body)) {}
    };

    struct ScopedBlockNode : public Node
    {
        std::vector<std::unique_ptr<Node>> statements_;

        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        ScopedBlockNode(std::vector<std::unique_ptr<Node>> statements) : statements_(std::move(statements)) {};
    };

    struct ProgramEntryNode : public Node
    {
        std::unique_ptr<Node> body;

        void Accept(INodeVisitor &visitor) override
        {
            visitor.Visit(this);
        }

        ProgramEntryNode(std::unique_ptr<Node> body) : body(std::move(body)) {}
    };
}

#endif //UNLOGIC_NODE_H
