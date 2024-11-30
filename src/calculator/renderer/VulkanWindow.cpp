#include "VulkanWindow.h"

using namespace ui;

void VulkanWindow::setScene(std::shared_ptr<unlogic::Scene> scene)
{
    this->scene = scene;
}

void VulkanWindow::mousePressEvent(QMouseEvent *ev)
{
    this->last_mouse_pos_ = ev->position();
}

void VulkanWindow::mouseMoveEvent(QMouseEvent *ev)
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

void VulkanWindow::wheelEvent(QWheelEvent *ev)
{
    float delta = 0.f;
    if (ev->hasPixelDelta())
    {
        delta = ev->pixelDelta().y() / 5.f;
    }
    else
    {
        delta = ev->angleDelta().y() / 15.f;
    }

    this->camera.TranslateWorld({0, 0, delta});
}

void VulkanWindow::resizeEvent(QResizeEvent *ev)
{
    this->camera.SetDPI((float)this->devicePixelRatio());

    this->camera.SetWindowSize({
            ev->size().width(),
            ev->size().height(),
    });
}

QVulkanWindowRenderer *VulkanWindow::createRenderer()
{
    this->renderer = new VulkanRenderer(this);

    return this->renderer;
}
