//
// Created by nathan on 6/18/24.
//

#ifndef UNLOGIC_LIBRARY_H
#define UNLOGIC_LIBRARY_H

#include <string>
#include <utility>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include "transformer/IRGenerationContext.h"

namespace unlogic
{
    struct LibrarySymbol;

    struct Library
    {
        std::string name;
        std::vector<LibrarySymbol*> symbols;

        Library(std::string name) : name(std::move(name)) {}

        Library() = delete;
    };

    struct LibrarySymbol
    {
        char const *name;
        llvm::orc::ExecutorSymbolDef symbol;

        using PopulateScopeType = std::function<void(llvm::LLVMContext&, llvm::Module&, Scope&)>;
        PopulateScopeType PopulateScope;

        LibrarySymbol(Library &lib, char const *name, void *native, PopulateScopeType populate_scope) : name(name), PopulateScope(std::move(populate_scope))
        {
            this->symbol = {
                    llvm::orc::ExecutorAddr::fromPtr(native),
                    llvm::JITSymbolFlags::Callable,
            };

            lib.symbols.push_back(this);
        };
    };
}

#endif //UNLOGIC_LIBRARY_H
