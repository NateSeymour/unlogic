#include "Node.h"

using namespace unlogic;

llvm::Value *VariableNode::GenerateIR(CompilationContext &ctx)
{
    return ctx.named_values[this->identifier_];
}

llvm::Value *NumericLiteralNode::GenerateIR(CompilationContext &ctx)
{
    return llvm::ConstantFP::get(*ctx.llvm_ctx, llvm::APFloat(this->value_));
}

llvm::Value *CallNode::GenerateIR(CompilationContext &ctx)
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
        argument_values.push_back(argument->GenerateIR(ctx));
    }

    return ctx.builder->CreateCall(function, argument_values, "calltmp");
}

llvm::Value *AdditionNode::GenerateIR(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->GenerateIR(ctx);
    llvm::Value *rhs = this->rhs_->GenerateIR(ctx);

    return ctx.builder->CreateFAdd(lhs, rhs, "addtmp");
}

llvm::Value *SubtractionNode::GenerateIR(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->GenerateIR(ctx);
    llvm::Value *rhs = this->rhs_->GenerateIR(ctx);

    return ctx.builder->CreateFSub(lhs, rhs, "subtmp");
}

llvm::Value *MultiplicationNode::GenerateIR(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->GenerateIR(ctx);
    llvm::Value *rhs = this->rhs_->GenerateIR(ctx);

    return ctx.builder->CreateFMul(lhs, rhs, "multmp");
}

llvm::Value *DivisionNode::GenerateIR(CompilationContext &ctx)
{
    llvm::Value *lhs = this->lhs_->GenerateIR(ctx);
    llvm::Value *rhs = this->rhs_->GenerateIR(ctx);

    return ctx.builder->CreateFDiv(lhs, rhs, "divtmp");
}

llvm::Value *PotentiationNode::GenerateIR(CompilationContext &ctx) {
    llvm::Value *lhs = this->lhs_->GenerateIR(ctx);
    llvm::Value *rhs = this->rhs_->GenerateIR(ctx);

    llvm::Function *pow = ctx.module->getFunction("pow");

    return ctx.builder->CreateCall(pow, {lhs, rhs}, "powtmp");
}
