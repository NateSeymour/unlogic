//
// Created by Nathan on 11/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include <format>
#include "CompilerController.h"
#include "renderer/VulkanInstance.h"
#include "renderer/VulkanWindow.h"
#include "util/overload.h"

namespace ui
{
    class Window : public QMainWindow
    {
        Q_OBJECT

        QTextEdit *editor_ = nullptr;
        VulkanWindow *render_window_ = nullptr;
        QWidget *renderer_widget_ = nullptr;
        QLabel *status_label_ = nullptr;

        CompilerController compiler_controller_;

        std::shared_ptr<unlogic::Scene> scene_;

    public Q_SLOTS:
        void editorTextChanged()
        {
            std::string program_text = this->editor_->toPlainText().toStdString();
            Q_EMIT compileAndRun(std::move(program_text));
        }

        void statusUpdate(ui::CompilationStatus status)
        {
            switch (status)
            {
                case CompilationStatus::InProgress:
                {
                    this->status_label_->setText("Compiling...");
                    break;
                }

                case CompilationStatus::Successful:
                {
                    this->status_label_->setText("Compilation Successful!");
                    break;
                }

                default:
                    break;
            }
        }

        void tokenizationComplete(std::vector<bf::Token<unlogic::ParserGrammarType>> tokens)
        {
            for (auto const &token: tokens)
            {
            }
        }

        void sceneReady(std::shared_ptr<unlogic::Scene> scene)
        {
            this->scene_ = std::move(scene);
            this->render_window_->setScene(this->scene_);
        }

        void compilationError(unlogic::Error error)
        {
            this->status_label_->setText(error.message.c_str());
        }

    Q_SIGNALS:
        void compileAndRun(std::string program_text);

    public:
        Window()
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
            QObject::connect(this->editor_, &QTextEdit::textChanged, this, &Window::editorTextChanged);
            this->editor_->setFontFamily("Source Code Pro");

            // Compiler Controller
            QObject::connect(this, &Window::compileAndRun, &this->compiler_controller_, &CompilerController::compileAndRun);
            QObject::connect(&this->compiler_controller_, &CompilerController::sceneReady, this, &Window::sceneReady);
            QObject::connect(&this->compiler_controller_, &CompilerController::tokenizationComplete, this, &Window::tokenizationComplete);
            QObject::connect(&this->compiler_controller_, &CompilerController::compilationError, this, &Window::compilationError);
            QObject::connect(&this->compiler_controller_, &CompilerController::statusUpdate, this, &Window::statusUpdate);

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
