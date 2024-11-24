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
        VulkanWindow *render_window_ = nullptr;
        VulkanRenderer *renderer_ = nullptr;
        QWidget *renderer_widget_ = nullptr;

        std::shared_ptr<unlogic::Scene> scene_;

    public slots:
        void on_run_button_pressed()
        {
            auto program_text = this->editor_->toPlainText().toStdString();

            unlogic::Compiler compiler({&unlogic::stdlib, &unlogic::runtime});
            auto program = compiler.Compile(program_text);

            this->scene_ = std::make_shared<unlogic::Scene>();
            program(this->scene_.get());

            this->render_window_->setScene(this->scene_);
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

            this->render_window_ = new VulkanWindow;
            this->scene_ = std::make_shared<unlogic::Scene>();
            this->render_window_->setVulkanInstance(ui::vk_global);
            this->render_window_->setScene(this->scene_);

            this->renderer_widget_ = QWidget::createWindowContainer(this->render_window_);
            splitter->addWidget(this->renderer_widget_);

            main_layout->addWidget(splitter, 100);

            body->setLayout(main_layout);

            this->setCentralWidget(body);
        }
    };
} // namespace ui

#endif // WINDOW_H
