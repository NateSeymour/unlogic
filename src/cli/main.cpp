#include <SFML/Graphics.hpp>
#include "../graphic/Graph.h"

int main()
{
    unlogic::Compiler::InitializeCompilerRuntime();

    // Construct GraphRenderer
    unlogic::Graph graph({ "z(x) := sin(x)", "a(x) := cos(x)", "b(x) := tan(x)" });

    // Construct window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Unlogic", sf::Style::Default, settings);

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
