#ifndef UNLOGIC_VULKANWINDOW_H
#define UNLOGIC_VULKANWINDOW_H

#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QVulkanWindow>
#include "VulkanRenderer.h"
#include "graphic/Scene.h"

namespace ui
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT

        friend class VulkanRenderer;

        QPointF last_mouse_pos_;

    public slots:
        void setScene(unlogic::Scene *scene);

    signals:
        void sceneChanged(unlogic::Scene *scene);

    protected:
        unlogic::Scene *scene = nullptr;

        void mousePressEvent(QMouseEvent *ev) override
        {
            this->last_mouse_pos_ = ev->position();
        }

        void mouseMoveEvent(QMouseEvent *ev) override
        {
            if (this->scene && ev->buttons() & Qt::LeftButton)
            {
                QPointF current_position = ev->position();

                this->scene->camera.center.x += (float)(current_position.x() - this->last_mouse_pos_.x());
                this->scene->camera.center.y += (float)(current_position.y() - this->last_mouse_pos_.y());

                this->last_mouse_pos_ = current_position;
            }
        }

        void wheelEvent(QWheelEvent *ev) override
        {
            this->scene->camera.resolution += (float)ev->angleDelta().y();
        }

        void resizeEvent(QResizeEvent *ev) override
        {
            this->scene->camera.dpi_scalar = (float)this->devicePixelRatio();

            this->scene->camera.window = {
                    ev->size().width(),
                    ev->size().height(),
            };
        }

    public:
        QVulkanWindowRenderer *createRenderer() override
        {
            return new VulkanRenderer(this);
        }
    };
} // namespace ui

#endif // UNLOGIC_VULKANWINDOW_H
