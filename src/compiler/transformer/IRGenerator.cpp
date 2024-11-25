#include <format>
#include "IRGenerator.h"

void unlogic::IRGenerator::Visit(unlogic::NumericLiteralNode const *node)
{
    llvm::Value *value = llvm::ConstantFP::get(this->ctx.llvm_ctx, llvm::APFloat(node->value));
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::StringLiteralNode const *node) {}

void unlogic::IRGenerator::Visit(unlogic::VariableNode const *node)
{
    llvm::Value *value = *this->ctx.scope.Lookup(node->identifier_);
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::CallNode const *node)
{
    llvm::Function *function = ctx.module->getFunction(node->function_name_);

    if (function->arg_size() < node->arguments_.size())
    {
        throw std::runtime_error("Aaaaaahhhhhhh");
    }

    std::vector<llvm::Value *> argument_values;
    argument_values.reserve(node->arguments_.size());
    for (auto const &argument: node->arguments_)
    {
        argument->Accept(*this);
        argument_values.push_back(this->values.top());
        this->values.pop();
    }

    llvm::Value *value = this->builder.CreateCall(function, argument_values, "calltmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::AdditionNode const *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFAdd(lhs, rhs, "addtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::SubtractionNode const *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFSub(lhs, rhs, "subtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::MultiplicationNode const *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFMul(lhs, rhs, "multmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::DivisionNode const *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Value *value = this->builder.CreateFDiv(lhs, rhs, "divtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::PotentiationNode const *node)
{
    node->lhs_->Accept(*this);
    llvm::Value *lhs = this->values.top();
    this->values.pop();

    node->rhs_->Accept(*this);
    llvm::Value *rhs = this->values.top();
    this->values.pop();

    llvm::Function *std_pow = this->ctx.module->getFunction("pow");

    llvm::Value *value = this->builder.CreateCall(std_pow, {lhs, rhs}, "powtmp");
    this->values.push(value);
}

void unlogic::IRGenerator::Visit(unlogic::FunctionDefinitionNode const *node)
{
    // Save entry
    llvm::BasicBlock *parent = this->builder.GetInsertBlock();

    // Generate function information
    std::vector<llvm::Type *> argument_types(node->args_.size(), llvm::Type::getDoubleTy(ctx.llvm_ctx));
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(ctx.llvm_ctx), argument_types, false);
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, node->name_, *ctx.module);

    this->ctx.scope.Insert(node->name_, function);

    unsigned idx = 0;
    for (auto &arg: function->args())
    {
        arg.setName(node->args_[idx++]);
    }

    // Generate function body
    llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.llvm_ctx, node->name_, function);
    this->builder.SetInsertPoint(block);

    ctx.scope.PushLayer();
    for (auto &arg: function->args())
    {
        ctx.scope.Insert(std::string(arg.getName()), &arg);
    }

    node->body_->Accept(*this);
    llvm::Value *return_value = this->values.top();
    this->values.pop();

    this->builder.CreateRet(return_value);

    ctx.scope.PopLayer();

    if (llvm::verifyFunction(*function, &llvm::errs()))
    {
        throw std::runtime_error("function has errors");
    }

    this->values.push(function);

    // Return to parent block
    this->builder.SetInsertPoint(parent);
}

void unlogic::IRGenerator::Visit(unlogic::PlotCommandNode const *node)
{
    llvm::Value *scene = *this->ctx.scope.Lookup("__scene");
    llvm::Value *name = this->builder.CreateGlobalStringPtr(node->function_name);

    auto function = this->ctx.module->getFunction(node->function_name);
    if (!function)
    {
        throw std::runtime_error(std::format("Function \"{}\" could not be found!", node->function_name));
    }

    auto scene_add_plot = this->ctx.module->getFunction("unlogic_scene_add_plot");

    std::array<llvm::Value *, 3> args = {scene, name, function};

    llvm::Value *ret = this->builder.CreateCall(scene_add_plot, args);

    this->values.push(ret);
}

void unlogic::IRGenerator::Visit(unlogic::ScopedBlockNode const *node)
{
    for (auto &statement: node->statements_)
    {
        statement->Accept(*this);
        this->values.pop();
    }
}

void unlogic::IRGenerator::Visit(unlogic::ProgramEntryNode const *node)
{
    std::array<llvm::Type *, 1> args = {
            llvm::PointerType::getUnqual(this->ctx.llvm_ctx),
    };
    llvm::FunctionType *entry_type = llvm::FunctionType::get(llvm::Type::getVoidTy(this->ctx.llvm_ctx), args, false);
    llvm::Function *entry = llvm::Function::Create(entry_type, llvm::Function::ExternalLinkage, "__entry", *this->ctx.module);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.llvm_ctx, "__entry", entry);

    this->ctx.scope.PushLayer();

    this->ctx.scope.Insert("__scene", entry->getArg(0));

    this->builder.SetInsertPoint(block);

    node->body->Accept(*this);

    this->builder.CreateRetVoid();
    this->ctx.scope.PopLayer();

    if (llvm::verifyFunction(*entry, &llvm::errs()))
    {
        throw std::runtime_error("function has errors");
    }
}
