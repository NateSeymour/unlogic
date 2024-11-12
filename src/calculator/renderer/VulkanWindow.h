#ifndef UNLOGIC_VULKANWINDOW_H
#define UNLOGIC_VULKANWINDOW_H

#include <QVulkanWindow>
#include "VulkanRenderer.h"

namespace ui
{
    class VulkanWindow : public QVulkanWindow
    {
    public:
        QVulkanWindowRenderer *createRenderer() override
        {
            return new VulkanRenderer(this);
        }
    };
} // ui

#endif //UNLOGIC_VULKANWINDOW_H
