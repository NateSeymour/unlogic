#include "CompilerController.h"
#include "CompilerWorker.h"

using namespace ui;

void CompilerController::compileAndRun(std::string program_text)
{
    Q_EMIT compilationRequest(std::move(program_text));
}

CompilerController::CompilerController()
{
    auto worker = new CompilerWorker;
    worker->moveToThread(&this->worker_thread_);

    QObject::connect(&this->worker_thread_, &QThread::finished, worker, &QObject::deleteLater);

    QObject::connect(this, &CompilerController::compilationRequest, worker, &CompilerWorker::compileAndRun);

    QObject::connect(worker, &CompilerWorker::sceneReady, this, &CompilerController::sceneReady);
    QObject::connect(worker, &CompilerWorker::statusUpdate, this, &CompilerController::statusUpdate);
    QObject::connect(worker, &CompilerWorker::compilationError, this, &CompilerController::compilationError);

    this->worker_thread_.start();
}

CompilerController::~CompilerController()
{
    this->worker_thread_.quit();
    this->worker_thread_.wait();
}
