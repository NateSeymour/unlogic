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

        llvm::Function *GenerateStdLibFunctionDefinition(std::string const &name, std::uint8_t nargs)
        {
            std::vector<llvm::Type*> argument_types(nargs, llvm::Type::getDoubleTy(*this->llvm_ctx));
            llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(*this->llvm_ctx), argument_types, false);
            return llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, name, *this->module);
        }

        void InitializeStdLib()
        {
            this->GenerateStdLibFunctionDefinition("pow", 2);
            this->GenerateStdLibFunctionDefinition("sin", 1);
        }

        CompilationContext(llvm::StringRef module_name = "Unlogic")
        {
            this->llvm_ctx = std::make_unique<llvm::LLVMContext>();
            this->builder = std::make_unique<llvm::IRBuilder<>>(*this->llvm_ctx);
            this->module = std::make_unique<llvm::Module>(module_name, *llvm_ctx);

            this->InitializeStdLib();
        }
    };

    enum class NodeType
    {
        Variable,
        Constant,
        Call,
        BinaryOperation,
    };

    class Node
    {
    public:
        virtual llvm::Value *Codegen(CompilationContext &ctx) = 0;
        virtual NodeType Type() const = 0;
        virtual std::vector<Node const *> Children() const = 0;

        virtual ~Node() {}
    };

    class VariableNode : public Node
    {
        friend class Parser;

    protected:
        std::string identifier_;

    public:
        llvm::Value *Codegen(CompilationContext &ctx) override
        {
            return ctx.named_values[this->identifier_];
        }

        NodeType Type() const override
        {
            return NodeType::Variable;
        }

        std::vector<Node const *> Children() const override
        {
            return { this };
        }

        VariableNode(std::string identifier) : identifier_(identifier) {}
    };

    class ConstantNode : public Node
    {
    protected:
        double value_ = 0.0;

    public:
        llvm::Value *Codegen(CompilationContext &ctx) override
        {
            return llvm::ConstantFP::get(*ctx.llvm_ctx, llvm::APFloat(this->value_));
        }

        NodeType Type() const override
        {
            return NodeType::Constant;
        }

        std::vector<const Node *> Children() const override
        {
            return { this };
        }

        ConstantNode(double value) : value_(value) {}
    };

    class CallNode : public Node
    {
        std::string function_name_;
        std::vector<std::unique_ptr<Node>> arguments_;

    public:
        llvm::Value *Codegen(CompilationContext &ctx) override
        {
            llvm::Function *function = ctx.module->getFunction(this->function_name_);

            if(function->arg_size() < this->arguments_.size())
            {
                throw std::runtime_error("Aaaaaahhhhhhh");
            }

            std::vector<llvm::Value *> argument_values;
            argument_values.reserve(this->arguments_.size());
            for(auto const &argument : this->arguments_)
            {
                argument_values.push_back(argument->Codegen(ctx));
            }

            return ctx.builder->CreateCall(function, argument_values, "calltmp");
        }

        NodeType Type() const override
        {
            return NodeType::Call;
        }

        std::vector<const Node *> Children() const override
        {
            std::vector<Node const *> children{this};
            for(auto const &argument : this->arguments_)
            {
                for(auto const &child : argument->Children())
                {
                    children.push_back(child);
                }
            }

            return children;
        }

        CallNode(std::string function_name, std::vector<std::unique_ptr<Node>> arguments) : function_name_(std::move(function_name)), arguments_(std::move(arguments)) {}
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

        std::vector<Node const *> Children() const override
        {
            std::vector<Node const *> children{this};
            for(auto const &child : this->lhs_->Children())
            {
                children.push_back(child);
            }

            for(auto const &child : this->rhs_->Children())
            {
                children.push_back(child);
            }

            return children;
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
