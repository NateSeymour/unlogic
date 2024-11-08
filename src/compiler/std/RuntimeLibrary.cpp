#include "RuntimeLibrary.h"

using namespace unlogic;

extern "C"
{
}

LibraryDefinition unlogic::runtime("runtime", [](Library &lib) {
    llvm::GlobalVariable rt(*lib.module,
                              llvm::PointerType::getVoidTy(lib.ctx),
                              false,
                              llvm::GlobalValue::LinkOnceAnyLinkage,
                              nullptr,
                              "__rt");
});
