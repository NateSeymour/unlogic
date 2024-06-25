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
    struct CompilationContext
    {
        std::unique_ptr<llvm::LLVMContext> llvm_ctx;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<llvm::IRBuilder<>> builder;

        std::map<std::string, llvm::Value*> named_values;

        llvm::Function *RegisterLibraryFunction(std::string const &name, std::uint8_t nargs)
        {
            std::vector<llvm::Type*> argument_types(nargs, llvm::Type::getDoubleTy(*this->llvm_ctx));
            llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(*this->llvm_ctx), argument_types, false);
            return llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, name, *this->module);
        }

        CompilationContext(llvm::StringRef module_name = "Unlogic")
        {
            this->llvm_ctx = std::make_unique<llvm::LLVMContext>();
            this->builder = std::make_unique<llvm::IRBuilder<>>(*this->llvm_ctx);
            this->module = std::make_unique<llvm::Module>(module_name, *llvm_ctx);
        }
    };

    enum class NodeType
    {
        Variable,
        Constant,
        Call,
        Addition,
        Subtraction,
        Multiplication,
        Division,
    };

    class Node
    {
    public:
        virtual llvm::Value *Codegen(CompilationContext &ctx) = 0;
        virtual NodeType Type() const = 0;
        virtual std::vector<Node const *> Children() const = 0;
        virtual std::unique_ptr<Node> Derive() const = 0;
        virtual std::unique_ptr<Node> Copy() const = 0;

        virtual ~Node() {}
    };

    class VariableNode : public Node
    {
        friend class Parser;

    protected:
        std::string identifier_;

    public:
        NodeType Type() const override
        {
            return NodeType::Variable;
        }

        llvm::Value *Codegen(CompilationContext &ctx) override;
        std::vector<Node const *> Children() const override;
        std::unique_ptr<Node> Derive() const override;
        std::unique_ptr<Node> Copy() const override;

        VariableNode(std::string identifier) : identifier_(identifier) {}
    };

    class ConstantNode : public Node
    {
    protected:
        double value_ = 0.0;

    public:
        NodeType Type() const override
        {
            return NodeType::Constant;
        }

        llvm::Value *Codegen(CompilationContext &ctx) override;
        std::vector<const Node *> Children() const override;
        std::unique_ptr<Node> Derive() const override;
        std::unique_ptr<Node> Copy() const override;

        ConstantNode(double value) : value_(value) {}
    };

    class CallNode : public Node
    {
        std::string function_name_;
        std::vector<std::unique_ptr<Node>> arguments_;

    public:
        NodeType Type() const override
        {
            return NodeType::Call;
        }

        llvm::Value *Codegen(CompilationContext &ctx) override;
        std::vector<const Node *> Children() const override;

        CallNode(std::string function_name, std::vector<std::unique_ptr<Node>> arguments) : function_name_(std::move(function_name)), arguments_(std::move(arguments)) {}
    };

    class BinaryNode : public Node
    {
    protected:
        std::unique_ptr<Node> lhs_, rhs_;

    public:
        std::vector<const Node *> Children() const override;

        BinaryNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
    };

    class AdditionNode : public BinaryNode
    {
    public:
        NodeType Type() const override
        {
            return NodeType::Addition;
        }

        std::unique_ptr<Node> Derive() const override;
        std::unique_ptr<Node> Copy() const override;
        llvm::Value *Codegen(CompilationContext &ctx) override;

        AdditionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class SubtractionNode : public BinaryNode
    {
    public:
        NodeType Type() const override
        {
            return NodeType::Subtraction;
        }

        std::unique_ptr<Node> Derive() const override;
        std::unique_ptr<Node> Copy() const override;
        llvm::Value *Codegen(CompilationContext &ctx) override;

        SubtractionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class MultiplicationNode : public BinaryNode
    {
    public:
        NodeType Type() const override
        {
            return NodeType::Multiplication;
        }

        std::unique_ptr<Node> Derive() const override;
        std::unique_ptr<Node> Copy() const override;
        llvm::Value *Codegen(CompilationContext &ctx) override;

        MultiplicationNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class DivisionNode : public BinaryNode
    {
    public:
        NodeType Type() const override
        {
            return NodeType::Division;
        }

        std::unique_ptr<Node> Derive() const override;
        std::unique_ptr<Node> Copy() const override;
        llvm::Value *Codegen(CompilationContext &ctx) override;

        DivisionNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs) : BinaryNode(std::move(lhs), std::move(rhs)) {}
    };

    class BinaryOperationNode : public Node
    {
        std::unique_ptr<Node> lhs_, rhs_;
        std::string op_;

    public:
        llvm::Value *Codegen(CompilationContext &ctx) override
        {
            llvm::Value *lhs = this->lhs_->Codegen(ctx);
            llvm::Value *rhs = this->rhs_->Codegen(ctx);

            if(op_ == "+") return ctx.builder->CreateFAdd(lhs, rhs, "addtmp");
            if(op_ == "-") return ctx.builder->CreateFSub(lhs, rhs, "subtmp");
            if(op_ == "*") return ctx.builder->CreateFMul(lhs, rhs, "multmp");
            if(op_ == "/") return ctx.builder->CreateFDiv(lhs, rhs, "divtmp");
            if(op_ == "^")
            {
                llvm::Function *pow = ctx.module->getFunction("pow");

                return ctx.builder->CreateCall(pow, {lhs, rhs}, "pow");
            }

            return nullptr;
        }

        NodeType Type() const override
        {
            return NodeType::BinaryOperation;
        }

        BinaryOperationNode(std::unique_ptr<Node> lhs, std::unique_ptr<Node> rhs, std::string op) : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(std::move(op)) {}
    };

    struct Prototype
    {
        std::unique_ptr<Node> body;
        std::string name;
        std::vector<std::string> arguments;

        bool anonymous = false;

        llvm::Function *Codegen(CompilationContext &ctx)
        {
            // Generate function information
            std::vector<llvm::Type*> argument_types(this->arguments.size(), llvm::Type::getDoubleTy(*ctx.llvm_ctx));
            llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(*ctx.llvm_ctx), argument_types, false);
            llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, this->name, *ctx.module);

            unsigned idx = 0;
            for (auto &arg : function->args())
            {
                arg.setName(this->arguments[idx++]);
            }

            // Generate function body
            llvm::BasicBlock *block = llvm::BasicBlock::Create(*ctx.llvm_ctx, "entry", function);
            ctx.builder->SetInsertPoint(block);

            ctx.named_values.clear();
            for(auto &arg : function->args())
            {
                ctx.named_values[std::string(arg.getName())] = &arg;
            }

            llvm::Value* return_value = this->body->Codegen(ctx);
            ctx.builder->CreateRet(return_value);
            llvm::verifyFunction(*function);

            return function;
        }
    };
}

#endif //UNLOGIC_NODE_H
