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

    public:
        QVulkanWindowRenderer *createRenderer() override
        {
            auto renderer = new VulkanRenderer(this);

            QObject::connect(this, &VulkanWindow::sceneChanged, renderer, &VulkanRenderer::setScene);

            return renderer;
        }

    public slots:
        void setScene(unlogic::Scene *scene);

    signals:
        void sceneChanged(unlogic::Scene *scene);
    };
} // ui

#endif //UNLOGIC_VULKANWINDOW_H
