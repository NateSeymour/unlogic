#ifndef UNLOGIC_VULKANWINDOW_H
#define UNLOGIC_VULKANWINDOW_H

#include <QVulkanWindow>
#include "VulkanRenderer.h"
#include "graphic/Scene.h"

namespace ui
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT

        friend class VulkanRenderer;

    protected:
        unlogic::Scene *scene = nullptr;

    public:
        QVulkanWindowRenderer *createRenderer() override
        {
            return new VulkanRenderer(this);
        }

    public slots:
        void setScene(unlogic::Scene *scene);

    signals:
        void sceneChanged(unlogic::Scene *scene);
    };
} // ui

#endif //UNLOGIC_VULKANWINDOW_H
