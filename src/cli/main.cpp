#include <SFML/Graphics.hpp>
#include "../graphic/Graph.h"

int main(int argc, char const **argv)
{
    unlogic::Compiler::InitializeCompilerRuntime();

    // Construct Graph
    unlogic::Graph graph;

    // Construct window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(2000, 1000), "Unlogic", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    float scalar = 100.f;

    sf::View view;
    view.setCenter(0.f, 0.f);
    view.setSize(window.getDefaultView().getSize() / scalar);
    window.setView(view);

    sf::Transform t;
    t.scale(1.0, -1.0);

    bool mouse_down = false;
    sf::Vector2i mouse_press_location;
    sf::Vector2f previous_center;

    sf::Clock delta;
    while(window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                {
                    window.close();
                    return 0;
                }

                case sf::Event::MouseWheelScrolled:
                {
                    scalar += (float)event.mouseWheelScroll.delta;
                    view.setSize(sf::Vector2f(window.getSize()) / scalar);
                    window.setView(view);
                    break;
                }

                case sf::Event::MouseButtonPressed:
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        mouse_down = true;
                        mouse_press_location = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        previous_center = view.getCenter();
                    }
                    break;
                }

                case sf::Event::MouseButtonReleased:
                {
                    if(event.mouseButton.button == sf::Mouse::Left)
                    {
                        mouse_down = false;
                    }
                    break;
                }

                case sf::Event::MouseMoved:
                {
                    if(!mouse_down) break;

                    sf::Vector2i current_location(event.mouseMove.x, event.mouseMove.y);
                    sf::Vector2i delta_pixels = mouse_press_location - current_location;
                    sf::Vector2f delta_coords = (sf::Vector2f(delta_pixels) / scalar) + previous_center;

                    view.setCenter(delta_coords.x, delta_coords.y);
                    window.setView(view);

                    break;
                }

                case sf::Event::Resized:
                {
                    view.setSize(sf::Vector2f((float)event.size.width, (float)event.size.height) / scalar);
                    window.setView(view);
                    break;
                }
            }
        }

        window.clear(sf::Color::White);

        window.draw(graph, t);

        window.display();
    }

    return 0;
}
