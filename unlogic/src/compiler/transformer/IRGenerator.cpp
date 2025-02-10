#include "IRGenerator.h"
#include <format>

city::Value *unlogic::IRGenerator::operator()(unlogic::NumericLiteralNode &node)
{
    return builder.CreateConstant<double>(node.value);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::StringLiteralNode &node)
{
    return nullptr;
    // return this->builder.CreateGlobalStringPtr(node.value);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::DivisionNode &node)
{
    city::Value *lhs = std::visit(*this, *node.lhs);
    city::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.InsertDivInst(lhs, rhs);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::ScopedBlockNode &node)
{
    for (auto &statement: node.statements)
    {
        std::visit(*this, *statement);
    }

    return nullptr;
}

city::Value *unlogic::IRGenerator::operator()(unlogic::VariableNode &node)
{
    return *this->ctx.scope.Lookup(node.identifier);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::CallNode &node)
{
    city::Function *function = this->ctx.functions.at(node.function_name);

    if (function->GetArgumentCount() < node.arguments.size())
    {
        throw std::runtime_error("Aaaaaahhhhhhh");
    }

    std::vector<city::Value *> arguments;
    arguments.reserve(node.arguments.size());
    for (auto &argument: node.arguments)
    {
        city::Value *arg_value = std::visit(*this, *argument);
        arguments.push_back(arg_value);
    }

    return this->builder.InsertCallInst(function, arguments);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::AdditionNode &node)
{
    city::Value *lhs = std::visit(*this, *node.lhs);
    city::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.InsertAddInst(lhs, rhs);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::SubtractionNode &node)
{
    city::Value *lhs = std::visit(*this, *node.lhs);
    city::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.InsertSubInst(lhs, rhs);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::MultiplicationNode &node)
{
    city::Value *lhs = std::visit(*this, *node.lhs);
    city::Value *rhs = std::visit(*this, *node.rhs);

    return this->builder.InsertMulInst(lhs, rhs);
}

city::Value *unlogic::IRGenerator::operator()(unlogic::PotentiationNode &node)
{
    city::Value *lhs = std::visit(*this, *node.lhs);
    city::Value *rhs = std::visit(*this, *node.rhs);

    city::Function *std_pow = this->ctx.module->getFunction("pow");

    return this->builder.CreateCall(std_pow, {lhs, rhs}, "powtmp");
}

city::Value *unlogic::IRGenerator::operator()(unlogic::FunctionDefinitionNode &node)
{
    // Save entry
    city::IRBlock &parent = this->builder.GetInsertPoint();

    // Generate function information
    std::vector<city::Type> argument_types(node.args.size(), city::Type::Get<double>());
    city::IRFunction *function = this->builder.CreateFunction(node.name, city::Type::Get<double>(), argument_types);

    this->ctx.functions[node.name] = function;

    ctx.scope.PushLayer();
    for (auto const &[value, name]: std::views::zip(function->GetArgs(), node.args))
    {
        ctx.scope.Set(name, value);
    }

    city::Value *retval = std::visit(*this, *node.body);

    this->builder.InsertRetInst(retval);

    ctx.scope.PopLayer();

    // Return to parent block
    this->builder.SetInsertPoint(parent);

    return nullptr;
}

city::Value *unlogic::IRGenerator::operator()(unlogic::PlotCommandNode &node)
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

city::Value *unlogic::IRGenerator::operator()(unlogic::ProgramEntryNode &node)
{
    std::array<llvm::Type *, 1> args = {
            llvm::PointerType::getUnqual(this->ctx.llvm_ctx),
    };
    llvm::FunctionType *entry_type = llvm::FunctionType::get(llvm::Type::getVoidTy(this->ctx.llvm_ctx), args, false);
    llvm::Function *entry = llvm::Function::Create(entry_type, llvm::Function::ExternalLinkage, "__entry", *this->ctx.module);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx.llvm_ctx, "__entry", entry);

    this->ctx.scope.PushLayer();

    this->ctx.scope.Set("__scene", entry->getArg(0));

    this->builder.SetInsertPoint(block);

    std::visit(*this, *node.body);

    this->builder.CreateRetVoid();
    this->ctx.scope.PopLayer();

    return nullptr;
}

city::Value *unlogic::IRGenerator::operator()(std::monostate &node)
{
    throw std::runtime_error("Invalid Node!");
}
