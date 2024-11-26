#ifndef UNLOGIC_VULKANWINDOW_H
#define UNLOGIC_VULKANWINDOW_H

#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QVulkanWindow>
#include "VulkanRenderer.h"
#include "VulkanVertexBuffer.h"
#include "graphic/Scene.h"
#include "graphic/shape/Rect.h"

namespace ui
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT

        friend class VulkanRenderer;

        QPointF last_mouse_pos_;

    public slots:
        void setScene(std::shared_ptr<unlogic::Scene> scene);

    signals:
        void sceneChanged(std::shared_ptr<unlogic::Scene> scene);

    protected:
        unlogic::Camera camera;
        unlogic::Rect grid = std::array{
                glm::vec2{-1.f, -1.f},
                glm::vec2{1.f, -1.f},
                glm::vec2{1.f, 1.f},
                glm::vec2{-1.f, 1.f},
        };
        std::shared_ptr<unlogic::Scene> scene;
        glm::vec2 world_mouse_coordinates = {0.f, 0.f};

        void mousePressEvent(QMouseEvent *ev) override
        {
            this->last_mouse_pos_ = ev->position();
        }

        void mouseMoveEvent(QMouseEvent *ev) override
        {
            if (this->scene && ev->buttons() & Qt::LeftButton)
            {
                QPointF current_position = ev->position();

                float dx = current_position.x() - this->last_mouse_pos_.x();
                float dy = current_position.y() - this->last_mouse_pos_.y();

                this->camera.TranslatePixel({dx, dy});

                this->last_mouse_pos_ = current_position;
            }
        }

        void wheelEvent(QWheelEvent *ev) override
        {
            float delta = ev->pixelDelta().y();
            this->camera.TranslateWorld({0, 0, delta / 5.f});
        }

        void resizeEvent(QResizeEvent *ev) override
        {
            this->camera.SetDPI((float)this->devicePixelRatio());

            this->camera.SetWindowSize({
                    ev->size().width(),
                    ev->size().height(),
            });
        }

    public:
        QVulkanWindowRenderer *createRenderer() override
        {
            return new VulkanRenderer(this);
        }
    };
} // namespace ui

#endif // UNLOGIC_VULKANWINDOW_H
