#ifndef UNLOGIC_PARENT_NOTEBOOK_H
#define UNLOGIC_PARENT_NOTEBOOK_H

#include <QTextEdit>
#include <memory>
#include "graphic/Scene.h"

namespace ui
{
    class Notebook : public QTextEdit
    {
        Q_OBJECT

        std::shared_ptr<unlogic::Scene> scene_;

    public Q_SLOTS:
        void setScene(std::shared_ptr<unlogic::Scene> scene);

    public:
        Notebook();
    };
} // namespace ui

#endif // UNLOGIC_PARENT_NOTEBOOK_H
