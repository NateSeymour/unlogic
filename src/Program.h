#ifndef UNLOGIC_PROGRAM_H
#define UNLOGIC_PROGRAM_H

#include "parser/Node.h"

namespace unlogic
{
    class Program
    {
        llvm::LLVMContext &llvm_ctx_;
        llvm::IRBuilder<> builder_;

        std::unique_ptr<Node> body_;

    public:
        llvm::Module Build()
        {
            llvm::Module module("unlogic", this->llvm_ctx_);

            // Create compilation context
            Scope program_scope;

            CompilationContext ctx = {
                    .llvm_ctx = this->llvm_ctx_,
                    .module = module,
                    .builder = this->builder_,
                    .scope = program_scope,
            };

            // Generate entry entry
            llvm::FunctionType *entry_type = llvm::FunctionType::get(llvm::Type::getVoidTy(this->llvm_ctx_), false);
            llvm::Function *entry = llvm::Function::Create(entry_type, llvm::Function::ExternalLinkage, "main", module);

            // Generate entry body
            llvm::BasicBlock *block = llvm::BasicBlock::Create(this->llvm_ctx_, "entry", entry);
            this->builder_.SetInsertPoint(block);

            llvm::Value* return_value = this->body_->GenerateIR();
            this->builder_.CreateRet(return_value);
            llvm::verifyFunction(*entry);

            return std::move(module);
        }

        Program() = delete;

        Program(llvm::LLVMContext &llvm_ctx, std::unique_ptr<Node> body)
            : body_(std::move(body)),
              llvm_ctx_(llvm_ctx),
              builder_(llvm_ctx) {}
    };
}

#endif //UNLOGIC_PROGRAM_H
