//
// Created by Nathan on 11/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <format>
#include "compiler/Compiler.h"
#include "compiler/std/RuntimeLibrary.h"
#include "compiler/std/StandardLibrary.h"
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "renderer/VulkanInstance.h"
#include "renderer/VulkanWindow.h"
#include "util/overload.h"

namespace ui
{
    class Window : public QMainWindow
    {
        QTextEdit *editor_ = nullptr;
        VulkanWindow *render_window_ = nullptr;
        VulkanRenderer *renderer_ = nullptr;
        QWidget *renderer_widget_ = nullptr;
        QLabel *status_label_ = nullptr;

        std::shared_ptr<unlogic::Scene> scene_;

        unlogic::Compiler compiler_;

    public slots:
        void on_editor_text_changed()
        {
            auto program_text = this->editor_->toPlainText().toStdString();

            this->status_label_->setText("Compiling...");
            auto program = compiler_.Compile(program_text);

            if (!program.has_value())
            {
                std::visit(unlogic::overload{
                       [&](unlogic::Error const &error) {
                           std::string message = std::format("Compilation Error: {}", error.message);
                           this->status_label_->setText(message.c_str());
                       },
                       [&](bf::Error const &error) {
                           std::string message = std::format("Parsing Error: {}", error.message);
                           this->status_label_->setText(message.c_str());
                       },
                       [&](llvm::Error const &error) { /* Do Nothing */ },
               }, program.error());
            }
            else
            {
                this->status_label_->setText("Compilation Successful!");
                this->scene_ = std::make_shared<unlogic::Scene>();
                program->operator()(this->scene_.get());

                this->render_window_->setScene(this->scene_);
            }
        }

    public:
        Window() : compiler_({&unlogic::stdlib, &unlogic::runtime})
        {
            auto body = new QWidget;
            auto main_layout = new QVBoxLayout;

            main_layout->setSpacing(0);
            main_layout->setContentsMargins(0, 0, 0, 0);

            // Main Area
            auto splitter = new QSplitter;

            // Status Label
            this->status_label_ = new QLabel;
            this->status_label_->setContentsMargins(5, 5, 5, 5);

            // Scene Rendering Window
            this->render_window_ = new VulkanWindow;
            this->scene_ = std::make_shared<unlogic::Scene>();
            this->render_window_->setVulkanInstance(ui::vk_global);
            this->render_window_->setScene(this->scene_);

            this->renderer_widget_ = QWidget::createWindowContainer(this->render_window_);

            // Editor
            this->editor_ = new QTextEdit;
            QObject::connect(this->editor_, &QTextEdit::textChanged, this, &Window::on_editor_text_changed);
            this->editor_->setFontFamily("Source Code Pro");

            // Layout Composition
            splitter->addWidget(this->editor_);
            splitter->addWidget(this->renderer_widget_);

            main_layout->addWidget(splitter, 100);
            main_layout->addWidget(this->status_label_);

            body->setLayout(main_layout);

            this->setCentralWidget(body);
        }
    };
} // namespace ui

#endif // WINDOW_H
