#ifndef UNLOGIC_SOURCEEDITOR_H
#define UNLOGIC_SOURCEEDITOR_H

#include <gtkmm.h>

namespace unlogic
{
    class SourceEditor : public Gtk::TextView
    {
        void on_source_editor_realize()
        {
            this->set_monospace(true);
            this->set_margin(3);
            this->set_wrap_mode(Gtk::WrapMode::WORD);
        }

    public:
        SourceEditor()
        {
            this->signal_realize().connect(sigc::mem_fun(*this, &SourceEditor::on_source_editor_realize));
        }
    };
}

#endif //UNLOGIC_SOURCEEDITOR_H
