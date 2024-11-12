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
        QWidget *renderer_ = nullptr;

    public:
        Window()
        {
            auto splitter = new QSplitter;

            this->editor_ = new QTextEdit;
            splitter->addWidget(this->editor_);

            auto render_window = new VulkanWindow;
            render_window->setVulkanInstance(ui::vk_global);

            this->renderer_ = QWidget::createWindowContainer(render_window);
            splitter->addWidget(this->renderer_);

            this->setCentralWidget(splitter);
        }
    };
}

#endif //WINDOW_H
