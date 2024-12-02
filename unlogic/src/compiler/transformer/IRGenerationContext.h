#ifndef UNLOGIC_IRGENERATIONCONTEXT_H
#define UNLOGIC_IRGENERATIONCONTEXT_H

#include <vector>
#include <optional>
#include <map>
#include <ranges>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>

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

    struct IRGenerationContext
    {
        llvm::LLVMContext &llvm_ctx;
        std::unique_ptr<llvm::Module> module;
        Scope &scope;
    };
}

#endif //UNLOGIC_IRGENERATIONCONTEXT_H
