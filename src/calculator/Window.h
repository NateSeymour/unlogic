//
// Created by Nathan on 11/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include "compiler/Compiler.h"
#include "compiler/std/RuntimeLibrary.h"
#include "compiler/std/StandardLibrary.h"
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include "renderer/VulkanInstance.h"
#include "renderer/VulkanVertexBuffer.h"
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

            unlogic::Compiler compiler({&unlogic::stdlib, &unlogic::runtime});
            auto program = compiler.Compile(program_text);

            this->scene_->plots.clear();
            program(this->scene_.get());
        }

    public:
        Window()
        {
            auto body = new QWidget;
            auto main_layout = new QVBoxLayout;

            // Top area
            auto run_button = new QPushButton("Run Program");
            QObject::connect(run_button, &QPushButton::pressed, this, &Window::on_run_button_pressed);
            main_layout->addWidget(run_button);

            // Main Area
            auto splitter = new QSplitter;

            this->editor_ = new QTextEdit;
            this->editor_->setFontFamily("Source Code Pro");
            this->editor_->setText("given f(x) := x^2;\nplot f;");
            splitter->addWidget(this->editor_);

            auto render_window = new VulkanWindow;
            auto vertex_buffer_provider = std::make_unique<ui::VulkanVertexBufferProvider>(render_window);
            this->scene_ = std::make_unique<unlogic::Scene>(std::move(vertex_buffer_provider));
            render_window->setVulkanInstance(ui::vk_global);
            render_window->setScene(this->scene_.get());

            this->renderer_widget_ = QWidget::createWindowContainer(render_window);
            splitter->addWidget(this->renderer_widget_);

            main_layout->addWidget(splitter, 100);

            body->setLayout(main_layout);

            this->setCentralWidget(body);
        }
    };
} // namespace ui

#endif // WINDOW_H
