
#ifndef UNLOGIC_COMPILERWORKER_H
#define UNLOGIC_COMPILERWORKER_H

#include <QObject>
#include <memory>
#include "CompilationStatus.h"
#include "Error.h"
#include "graphic/Scene.h"

/*
 * This forward declaration and unique pointer nonsense is required, else Qt's MOC absolutely shits the bed.
 * Probably because it doesn't understand LLVM or buffalo headers.
 */

namespace unlogic
{
    class Compiler;
}

namespace ui
{
    class CompilerWorker : public QObject
    {
        Q_OBJECT

        std::unique_ptr<unlogic::Compiler> compiler_;

    public Q_SLOTS:
        void compileAndRun(std::string program_text);

    Q_SIGNALS:
        void sceneReady(std::shared_ptr<unlogic::Scene> scene);
        void statusUpdate(ui::CompilationStatus status);
        void compilationError(unlogic::Error error);

    public:
        CompilerWorker();
        ~CompilerWorker();
    };
} // namespace ui

#endif // UNLOGIC_COMPILERWORKER_H
