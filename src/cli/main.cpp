#include <SFML/Graphics.hpp>
#include "../graphic/Graph.h"

int main()
{
    unlogic::Compiler::InitializeCompilerRuntime();

    // Construct GraphRenderer
    unlogic::Graph graph({
        "a(x) := sin(x)",
        "b(x) := cos(x)",
        "c(x) := tan(x)",
        "d(x) := sqrt(x)",
        "e(x) := log(x)",
    });

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
