//
// Created by Nathan on 11/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <QGridLayout>
#include <QMainWindow>
#include <QTextEdit>
#include <QSplitter>
#include "renderer/VulkanInstance.h"
#include "renderer/VulkanWindow.h"

namespace ui
{
    class Window : public QMainWindow
    {
        QTextEdit *editor_ = nullptr;
        VulkanRenderer *renderer_ = nullptr;
        QWidget *renderer_widget_ = nullptr;

        std::unique_ptr<unlogic::Scene> scene_;

    public:
        Window()
        {
            this->scene_ = std::make_unique<unlogic::Scene>();

            auto splitter = new QSplitter;

            this->editor_ = new QTextEdit;
            splitter->addWidget(this->editor_);

            auto render_window = new VulkanWindow;
            render_window->setVulkanInstance(ui::vk_global);
            this->scene_->background = unlogic::Color::Green;
            render_window->setScene(this->scene_.get());

            this->renderer_widget_ = QWidget::createWindowContainer(render_window);
            splitter->addWidget(this->renderer_widget_);

            this->setCentralWidget(splitter);
        }
    };
}

#endif //WINDOW_H
