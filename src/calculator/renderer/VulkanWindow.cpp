#include "VulkanWindow.h"

using namespace ui;

void VulkanWindow::setScene(unlogic::Scene *scene)
{
    if(this->scene != scene)
    {
        this->scene = scene;
        this->requestUpdate();
        emit sceneChanged(scene);
    }
}
