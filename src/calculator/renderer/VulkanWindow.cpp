#include "VulkanWindow.h"

using namespace ui;

void VulkanWindow::setScene(std::shared_ptr<unlogic::Scene> scene)
{
    this->scene = scene;
    this->requestUpdate();
    emit sceneChanged(scene);
}
