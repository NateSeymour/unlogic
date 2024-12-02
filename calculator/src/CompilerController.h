#ifndef UNLOGIC_COMPILERCONTROLLER_H
#define UNLOGIC_COMPILERCONTROLLER_H

#include <QObject>
#include <QThread>
#include <expected>
#include <memory>
#include "CompilationStatus.h"
#include "Error.h"
#include "graphic/Scene.h"
#include "parser/Parser.h"

namespace ui
{
    class CompilerController : public QObject
    {
        Q_OBJECT

        QThread worker_thread_;

    public Q_SLOTS:
        void compileAndRun(std::string program_text);

    Q_SIGNALS:
        void sceneReady(std::shared_ptr<unlogic::Scene> scene);
        void tokenizationComplete(std::vector<bf::Token<unlogic::ParserGrammarType>> tokens);
        void statusUpdate(ui::CompilationStatus status);
        void compilationError(unlogic::Error error);
        void compilationRequest(std::string program);

    public:
        CompilerController();
        ~CompilerController();
    };
} // namespace ui

#endif // UNLOGIC_COMPILERCONTROLLER_H
