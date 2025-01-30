#ifndef UNLOGIC_IRGENERATOR_H
#define UNLOGIC_IRGENERATOR_H

#include <city/ir/IRBuilder.h>
#include <city/ir/IRModule.h>
#include "IRGenerationContext.h"
#include "parser/Node.h"

namespace unlogic
{
    struct IRGenerator
    {
        IRGenerationContext &ctx;
        city::IRBuilder builder;

        city::Value *operator()(std::monostate &node);
        city::Value *operator()(NumericLiteralNode &node);
        city::Value *operator()(StringLiteralNode &node);
        city::Value *operator()(DivisionNode &node);
        city::Value *operator()(ScopedBlockNode &node);
        city::Value *operator()(VariableNode &node);
        city::Value *operator()(CallNode &node);
        city::Value *operator()(AdditionNode &node);
        city::Value *operator()(SubtractionNode &node);
        city::Value *operator()(MultiplicationNode &node);
        city::Value *operator()(PotentiationNode &node);
        city::Value *operator()(FunctionDefinitionNode &node);
        city::Value *operator()(PlotCommandNode &node);
        city::Value *operator()(ProgramEntryNode &node);

        IRGenerator(IRGenerationContext &ctx) : ctx(ctx), builder(ctx.module.CreateBuilder()) {}
    };
} // namespace unlogic

#endif // UNLOGIC_IRGENERATOR_H
