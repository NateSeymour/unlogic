#include "VulkanWindow.h"

#include <iostream>

using namespace ui;

void VulkanWindow::setScene(unlogic::Scene *scene)
{
    emit sceneChanged(scene);
}
