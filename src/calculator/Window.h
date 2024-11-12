//
// Created by Nathan on 11/12/2024.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <QGridLayout>
#include <QMainWindow>
#include <QTextEdit>

namespace ui
{
    class Window : public QMainWindow
    {
        QGridLayout *layout_ = nullptr;
        QTextEdit *editor_ = nullptr;

    public:
        Window()
        {
            this->layout_ = new QGridLayout;

            this->editor_ = new QTextEdit;
            this->layout_->addWidget(this->editor_);

            this->setLayout(this->layout_);
        }
    };
}

#endif //WINDOW_H
