#include <SFML/Graphics.hpp>
#include "GraphRenderer.test.h"

int main()
{
    // Construct GraphRenderer
    GraphRenderer renderer("f[x] = x^2;");

    // Construct window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(2400, 2400), "My window", sf::Style::Default, settings);

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

        renderer.Draw(window);
        window.display();
    }

    return 0;
}
