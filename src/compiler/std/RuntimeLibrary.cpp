#include "RuntimeLibrary.h"
#include "graphic/Scene.h"

using namespace unlogic;

extern "C"
{
    void unlogic_scene_add_plot(Scene *scene, char const *name, Plot2dFunctionType function)
    {
        scene->AddPlot(name, function);
    }
}

Library unlogic::runtime("runtime");

LibrarySymbol scene_add_plot(runtime, "unlogic_scene_add_plot", (void *)unlogic_scene_add_plot, [](llvm::LLVMContext &ctx, llvm::Module &mod, Scope &scope) {
    std::array<llvm::Type *, 1> plot_function_args = {
            llvm::Type::getDoubleTy(ctx),
    };
    llvm::FunctionType *plot_function_type = llvm::FunctionType::get(llvm::Type::getDoubleTy(ctx), plot_function_args, false);

    std::array<llvm::Type *, 3> args = {
            llvm::PointerType::getUnqual(ctx),
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(ctx)),
            llvm::PointerType::getUnqual(plot_function_type),
    };
    llvm::Type *ret = llvm::Type::getVoidTy(ctx);

    llvm::FunctionType *fn = llvm::FunctionType::get(ret, args, false);
    scope.Insert("scene_add_plot", llvm::Function::Create(fn, llvm::GlobalValue::ExternalLinkage, "unlogic_scene_add_plot", mod));
});
