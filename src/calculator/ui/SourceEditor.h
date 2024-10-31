#ifndef UNLOGIC_SOURCEEDITOR_H
#define UNLOGIC_SOURCEEDITOR_H

#include <iostream>
#include <gtkmm.h>
#include "parser/Parser.h"

namespace unlogic
{
    class SourceBuffer : public Gtk::TextBuffer
    {
        std::map<bf::Terminal<ParserGrammarType>, std::shared_ptr<Gtk::TextTag>> tag_lookup_;

        void on_source_buffer_changed()
        {
            this->remove_all_tags(this->begin(), this->end());

            auto buffer_text = this->get_text();
            auto token_stream = tokenizer.StreamInput(buffer_text.c_str());

            while(true)
            {
                auto token = token_stream.Consume();
                if(!token || token->terminal == tokenizer.EOS()) break;
                if(!this->tag_lookup_.contains(token->terminal)) continue;

                auto tag_begin = this->get_iter_at_offset((int)token->location.begin);
                auto tag_end = this->get_iter_at_offset((int)token->location.end);

                this->apply_tag(this->tag_lookup_[token->terminal], tag_begin, tag_end);
            }
        }

    public:
        SourceBuffer()
        {
            // Create tags
            auto tag_table = this->get_tag_table();

            std::map<SyntaxHighlightingGroup, Gdk::RGBA> colors = {
                    { SyntaxHighlightingGroup::Keyword, Gdk::RGBA(0.f, 1.f, 0.f, 1.f) },
                    { SyntaxHighlightingGroup::Operator, Gdk::RGBA(1.f, 0.f, 0.f, 1.f) },
                    { SyntaxHighlightingGroup::Identifier, Gdk::RGBA(0.f, 0.f, 1.f, 1.f) },
            };

            for(auto const &[terminal, group] : syntax_highlighting_groups)
            {
                auto tag = Gtk::TextTag::create();
                tag->property_foreground_rgba().set_value(colors[group]);
                tag_table->add(tag);

                this->tag_lookup_[terminal] = tag;
            }

            this->signal_changed().connect(sigc::mem_fun(*this, &SourceBuffer::on_source_buffer_changed));
        }
    };

    class SourceEditor : public Gtk::TextView
    {
        /*
         * Notes:
         * 1. GtkTextTagTable
         *
         */

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
