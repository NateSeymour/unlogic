//
// Created by Nathan on 11/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include "compiler/Compiler.h"
#include "compiler/std/RuntimeLibrary.h"
#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>
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

    public slots:
        void on_run_button_pressed()
        {
            auto program_text = this->editor_->toPlainText().toStdString();

            unlogic::Compiler compiler({ &unlogic::stdlib, &unlogic::runtime });
            auto program = compiler.Compile(program_text);

            program(this->scene_.get());
        }

    public:
        Window()
        {
            this->scene_ = std::make_unique<unlogic::Scene>();

            auto body = new QWidget;
            auto main_layout = new QVBoxLayout;

            // Top area
            auto run_button = new QPushButton("Run Program");
            QObject::connect(run_button, &QPushButton::pressed, this, &Window::on_run_button_pressed);
            main_layout->addWidget(run_button);

            // Main Area
            auto splitter = new QSplitter;

            this->editor_ = new QTextEdit;
            splitter->addWidget(this->editor_);

            auto render_window = new VulkanWindow;
            render_window->setVulkanInstance(ui::vk_global);
            this->scene_->background = unlogic::Color::Blue;
            render_window->setScene(this->scene_.get());

            this->renderer_widget_ = QWidget::createWindowContainer(render_window);
            splitter->addWidget(this->renderer_widget_);

            main_layout->addWidget(splitter, 100);

            body->setLayout(main_layout);

            this->setCentralWidget(body);
        }
    };
}

#endif //WINDOW_H
