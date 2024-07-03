//
// Created by nathan on 7/3/24.
//

#ifndef UNLOGIC_SFMLWIDGET_H
#define UNLOGIC_SFMLWIDGET_H

#include <gtkmm.h>
#include <SFML/Graphics.hpp>

namespace unlogic
{
    class SFMLWidget : public Gtk::GLArea, public sf::RenderTarget
    {
    public:
        sf::Vector2u getSize() const override
        {
            return {(unsigned)this->get_width(), (unsigned)this->get_height()};
        }
    };
} // unlogic

#endif //UNLOGIC_SFMLWIDGET_H
