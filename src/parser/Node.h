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
    class Scope
    {
        std::vector<std::map<std::string, llvm::Value*>> layers;

    public:
        std::optional<llvm::Value*> Lookup(std::string const &key)
        {
            for(auto &layer : this->layers | std::ranges::views::reverse)
            {
                if(layer.contains(key)) return layer[key];
            }

            return std::nullopt;
        }

        void Insert(std::string const &key, llvm::Value *value)
        {
            this->layers.back()[key] = value;
        }

        void PushLayer()
        {
            this->layers.emplace_back();
        }

        void PopLayer()
        {
            this->layers.pop_back();
        }
    };

    struct CompilationContext
    {
        llvm::LLVMContext &llvm_ctx;
        llvm::Module &module;
        llvm::IRBuilder<> &builder;
        Scope &scope;
    };

    class Node
    {
    public:
        virtual llvm::Value *GenerateIR(CompilationContext &ctx) = 0;

        virtual ~Node() = default;
    };

    template<typename T>
    class Literal
    {
    protected:
        T value_;

    public:
        virtual T Value() const { return this->value_; }

        virtual ~Literal() = default;

        Literal(T value) : value_(std::move(value)) {}
    };

    class NumericLiteralNode : public Node, public Literal<double>
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        NumericLiteralNode(double value) : Literal(value) {}
    };

    class StringLiteralNode : public Node, public Literal<std::string>
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override
        {
            return nullptr;
        }

        StringLiteralNode(std::string value) : Literal(std::move(value)) {}
    };

    class VariableNode : public Node
    {
        friend class Parser;

    protected:
        std::string identifier_;

    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        VariableNode(std::string identifier) : identifier_(std::move(identifier)) {}
    };

    class CallNode : public Node
    {
        std::string function_name_;
        std::vector<std::unique_ptr<Node>> arguments_;

    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        CallNode(std::string function_name, std::vector<std::unique_ptr<Node>> arguments) : function_name_(std::move(function_name)), arguments_(std::move(arguments)) {}
    };

    class BinaryNode : public Node
    {
    protected:
        std::unique_ptr<Node> lhs_, rhs_;

    public:
        BinaryNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    };

    class AdditionNode : public BinaryNode
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        AdditionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class SubtractionNode : public BinaryNode
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        SubtractionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class MultiplicationNode : public BinaryNode
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        MultiplicationNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class DivisionNode : public BinaryNode
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        DivisionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class PotentiationNode : public BinaryNode
    {
    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override;

        PotentiationNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class FunctionDefinitionNode : public Node
    {
        std::string name_;
        std::vector<std::string> args_;
        std::unique_ptr<Node> body_;

    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override
        {
            // Generate function information
            std::vector<llvm::Type*> argument_types(this->args_.size(), llvm::Type::getDoubleTy(ctx.llvm_ctx));
            llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(ctx.llvm_ctx), argument_types, false);
            llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, this->name_, ctx.module);

            unsigned idx = 0;
            for (auto &arg : function->args())
            {
                arg.setName(this->args_[idx++]);
            }

            // Generate function body
            llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.llvm_ctx, name_, function);
            ctx.builder.SetInsertPoint(block);

            ctx.scope.PushLayer();
            for(auto &arg : function->args())
            {
                ctx.scope.Insert(std::string(arg.getName()),  &arg);
            }

            llvm::Value* return_value = this->body_->GenerateIR(ctx);
            ctx.builder.CreateRet(return_value);

            ctx.scope.PopLayer();

            llvm::verifyFunction(*function);

            return function;
        }

        FunctionDefinitionNode(std::string name, std::vector<std::string> arguments, std::unique_ptr<Node> body)
            : name_(std::move(name)),
              args_(std::move(arguments)),
              body_(std::move(body)) {}
    };

    class ScopedBlockNode : public Node
    {
        std::vector<std::unique_ptr<Node>> statements_;

    public:
        llvm::Value *GenerateIR(CompilationContext &ctx) override
        {
            return nullptr;
        }

        ScopedBlockNode(std::vector<std::unique_ptr<Node>> statements) : statements_(std::move(statements)) {};
    };
}

#endif //UNLOGIC_NODE_H
