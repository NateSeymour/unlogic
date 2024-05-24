#include <iostream>
#include <SFML/Graphics.hpp>
#include "MathFunction.h"

int main()
{
    EvaluationContext context = {
            .parameters = {},
    };
    MathFunction eq("f[x, y] = x * y; f(bigboy, f(2, 2));");

    double value = eq.Evaluate(context);

    std::cout << value << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while(window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        window.display();
    }

    return 0;
}
