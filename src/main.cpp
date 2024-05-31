#include <SFML/Graphics.hpp>
#include "Graph.h"

int main()
{
    // Construct GraphRenderer
    Graph graph({ "f[x] = x^2;", "f[x] = x^3;", "f[x] = 2*x + 3" });

    // Construct window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(250, 250), "My window", sf::Style::Default, settings);

    sf::Transform t;
    t.scale(1.0, -1.0);

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
