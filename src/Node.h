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

namespace unlogic
{
    struct CompilationContext
    {
        llvm::LLVMContext llvm_ctx;
        llvm::IRBuilder<> builder;
        llvm::Module module;

        CompilationContext() : builder(llvm_ctx), module("Unlogic", llvm_ctx) {}
        CompilationContext(llvm::StringRef module_name) : builder(llvm_ctx), module(module_name, llvm_ctx) {}
    };

    class Node
    {
    public:
        virtual llvm::Value *Codegen(CompilationContext &ctx) = 0;

        virtual ~Node() {}
    };

    class BlockNode : public Node
    {

    };
}

#endif //UNLOGIC_NODE_H
