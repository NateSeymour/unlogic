#include "Notebook.h"

using namespace ui;

void Notebook::setScene(std::shared_ptr<unlogic::Scene> scene)
{
    this->scene_ = std::move(scene);
}

Notebook::Notebook()
{
    this->setReadOnly(true);
}
