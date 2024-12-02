#ifndef UNLOGIC_VULKANWINDOW_H
#define UNLOGIC_VULKANWINDOW_H

#include <QMouseEvent>
#include <QResizeEvent>
#include <QVulkanWindow>
#include <QWheelEvent>
#include "VulkanRenderer.h"
#include "graphic/Scene.h"
#include "graphic/shape/Rect.h"

namespace ui
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT

        friend class VulkanRenderer;

        QPointF last_mouse_pos_;

    public Q_SLOTS:
        void setScene(std::shared_ptr<unlogic::Scene> scene);

    protected:
        VulkanRenderer * renderer = nullptr;

        unlogic::Camera camera;
        unlogic::Rect grid = std::array{
                glm::vec2{-1.f, -1.f},
                glm::vec2{1.f, -1.f},
                glm::vec2{1.f, 1.f},
                glm::vec2{-1.f, 1.f},
        };
        std::shared_ptr<unlogic::Scene> scene;
        glm::vec2 world_mouse_coordinates = {0.f, 0.f};

        void mousePressEvent(QMouseEvent *ev) override;
        void mouseMoveEvent(QMouseEvent *ev) override;
        void wheelEvent(QWheelEvent *ev) override;
        void resizeEvent(QResizeEvent *ev) override;

    public:
        QVulkanWindowRenderer *createRenderer() override;
    };
} // namespace ui

#endif // UNLOGIC_VULKANWINDOW_H
