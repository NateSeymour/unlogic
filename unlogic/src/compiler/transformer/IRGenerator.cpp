#include "IRGenerator.h"
#include <format>
#include <llvm/IR/Verifier.h>

llvm::Value *unlogic::IRGenerator::operator()(unlogic::NumericLiteralNode &node)
{
    return llvm::ConstantFP::get(this->ctx.llvm_ctx, llvm::APFloat(node.value));
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::StringLiteralNode &node)
{
    return this->builder.CreateGlobalStringPtr(node.value);
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::DivisionNode &node)
{
    llvm::Value *lhs = std::visit(*this, *node.lhs);
    llvm::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.CreateFDiv(lhs, rhs, "divtmp");
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::ScopedBlockNode &node)
{
    for (auto &statement: node.statements)
    {
        std::visit(*this, *statement);
    }

    return nullptr;
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::VariableNode &node)
{
    return *this->ctx.scope.Lookup(node.identifier);
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::CallNode &node)
{
    llvm::Function *function = ctx.module->getFunction(node.function_name);

    if (function->arg_size() < node.arguments.size())
    {
        throw std::runtime_error("Aaaaaahhhhhhh");
    }

    std::vector<llvm::Value *> argument_values;
    argument_values.reserve(node.arguments.size());
    for (auto &argument: node.arguments)
    {
        llvm::Value *arg_value = std::visit(*this, *argument);
        argument_values.push_back(arg_value);
    }

    return this->builder.CreateCall(function, argument_values, "calltmp");
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::AdditionNode &node)
{
    llvm::Value *lhs = std::visit(*this, *node.lhs);
    llvm::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.CreateFAdd(lhs, rhs, "addtmp");
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::SubtractionNode &node)
{
    llvm::Value *lhs = std::visit(*this, *node.lhs);
    llvm::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.CreateFSub(lhs, rhs, "subtmp");
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::MultiplicationNode &node)
{
    llvm::Value *lhs = std::visit(*this, *node.lhs);
    llvm::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.CreateFMul(lhs, rhs, "multmp");
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::PotentiationNode &node)
{
    llvm::Value *lhs = std::visit(*this, *node.lhs);
    llvm::Value *rhs = std::visit(*this, *node.rhs);

    llvm::Function *std_pow = this->ctx.module->getFunction("pow");

    return this->builder.CreateCall(std_pow, {lhs, rhs}, "powtmp");
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::FunctionDefinitionNode &node)
{
    // Save entry
    llvm::BasicBlock *parent = this->builder.GetInsertBlock();

    // Generate function information
    std::vector<llvm::Type *> argument_types(node.args.size(), llvm::Type::getDoubleTy(ctx.llvm_ctx));
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(ctx.llvm_ctx), argument_types, false);
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, node.name, *ctx.module);

    this->ctx.scope.Insert(node.name, function);

    unsigned idx = 0;
    for (auto &arg: function->args())
    {
        arg.setName(node.args[idx++]);
    }

    // Generate function body
    llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.llvm_ctx, node.name, function);
    this->builder.SetInsertPoint(block);

    ctx.scope.PushLayer();
    for (auto &arg: function->args())
    {
        ctx.scope.Insert(std::string(arg.getName()), &arg);
    }

    llvm::Value *return_value = std::visit(*this, *node.body);

    this->builder.CreateRet(return_value);

    ctx.scope.PopLayer();

    if (llvm::verifyFunction(*function, &llvm::errs()))
    {
        throw std::runtime_error("function has errors");
    }

    // Return to parent block
    this->builder.SetInsertPoint(parent);

    return function;
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::PlotCommandNode &node)
{
    llvm::Value *scene = *this->ctx.scope.Lookup("__scene");
    llvm::Value *name = this->builder.CreateGlobalStringPtr(node.function_name);

    auto function = this->ctx.module->getFunction(node.function_name);
    if (!function)
    {
        throw std::runtime_error(std::format("Function \"{}\" could not be found!", node.function_name));
    }

    auto scene_add_plot = this->ctx.module->getFunction("unlogic_scene_add_plot");

    std::array<llvm::Value *, 3> args = {scene, name, function};

    return this->builder.CreateCall(scene_add_plot, args);
}

llvm::Value *unlogic::IRGenerator::operator()(unlogic::ProgramEntryNode &node)
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

    std::visit(*this, *node.body);

    this->builder.CreateRetVoid();
    this->ctx.scope.PopLayer();

    if (llvm::verifyFunction(*entry, &llvm::errs()))
    {
        throw std::runtime_error("function has errors");
    }

    return nullptr;
}

llvm::Value *unlogic::IRGenerator::operator()(std::monostate &node)
{
    throw std::runtime_error("Invalid Node!");
}
