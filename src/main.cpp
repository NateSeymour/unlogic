#include <iostream>
#include <SFML/Graphics.hpp>
#include "MathFunction.h"
#include "format.h"

int main()
{
    EvaluationContext context = {
            .parameters = {},
    };
    MathFunction eq("f[x] = x^2; f(point);");

    // Construct window
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    // Establish view
    sf::View view;
    window.setView(view);

    // Fonts and text
    sf::Font font;
    if(!font.loadFromFile("resource/Roboto-Medium.ttf"))
    {
        throw std::runtime_error("Failed to find font!");
    }

    // Calculate graph
    sf::Vector2f range(-20.f, 20.f);
    float step = 0.1f;
    std::uint64_t vertex_count = ceil((abs(range.x) + abs(range.y)) / step) + 1;

    std::vector<sf::Vertex> line;
    float x = range.x;
    for(int i = 0; i < vertex_count; i++)
    {
        context.parameters["point"] = x;

        auto value = (float)eq.Evaluate(context);
        std::cout << x << "^2" << " = " << value << std::endl;

        line.emplace_back(sf::Vector2f((float)x, value * -1), sf::Color::Red);

        x += step;
    }

    // run the program as long as the window is open
    sf::Vector2f mouse_coords;
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

                case sf::Event::Resized:
                case sf::Event::GainedFocus:
                {
                    auto size = window.getSize();

                    float width = (float)size.x;
                    float height = (float)size.y;

                    view.reset(sf::FloatRect(width * -0.5f, height * -0.5f, width, height));
                    window.setView(view);
                    break;
                }

                case sf::Event::MouseMoved:
                {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    mouse_coords = window.mapPixelToCoords(mouse, view);
                    break;
                }
            }
        }

        window.clear(sf::Color::Black);

        // calculate value
        context.parameters["point"] = mouse_coords.x;
        double value = eq.Evaluate(context);

        // Draw vertical line
        sf::Vertex vertical_line[] =
        {
                sf::Vertex(sf::Vector2f(mouse_coords.x, -1000.f), sf::Color::White),
                sf::Vertex(sf::Vector2f(mouse_coords.x, 1000.f), sf::Color::White),
        };

        window.draw(vertical_line, 2, sf::Lines);

        // Draw horizontal line
        sf::Vertex horizontal_line[] =
        {
                sf::Vertex(sf::Vector2f(-1000.f, (float)value * -1), sf::Color::White),
                sf::Vertex(sf::Vector2f(1000.f, (float)value * -1), sf::Color::White),
        };

        window.draw(horizontal_line, 2, sf::Lines);

        // Draw Cursor
        sf::CircleShape cursor(1.f);
        cursor.setPosition(mouse_coords.x - 1.f, mouse_coords.y - 1.f);
        cursor.setFillColor(sf::Color::Blue);

        window.draw(cursor);

        // Draw text
        sf::Text graph_value;
        graph_value.setString(nys::format("f(%f) = %f", mouse_coords.x, value));
        graph_value.setFillColor(sf::Color::White);
        graph_value.setCharacterSize(12);
        graph_value.setFont(font);
        graph_value.setPosition(0, 0);

        window.draw(graph_value);

        // Draw graph
        window.draw(line.data(), line.size(), sf::LineStrip);

        window.display();
    }

    return 0;
}
