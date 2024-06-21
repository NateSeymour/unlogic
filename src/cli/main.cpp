#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "../graphic/Graph.h"

int main(int argc, char const **argv)
{
    unlogic::Compiler::InitializeCompilerRuntime();

    // Construct Graph
    unlogic::Graph graph;

    // Parse Arguments
    for(int i = 1; i < argc; i++)
    {
        graph.AddPlot(std::string(argv[i]));
    }

    // Construct window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Unlogic", sf::Style::Default, settings);

    float scalar = 100.f;

    sf::View view;
    view.setCenter(0.f, 0.f);
    view.setSize(window.getDefaultView().getSize() / scalar);
    window.setView(view);

    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    ImGuiIO &io = ImGui::GetIO();
    io.FontGlobalScale = 2.f;

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
            ImGui::SFML::ProcessEvent(event);

            switch(event.type)
            {
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

                    auto const &center = view.getCenter();
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

                case sf::Event::Closed:
                {
                    window.close();
                    return 0;
                }
            }
        }

        ImGui::SFML::Update(window, delta.restart());

        if(mouse_down)
        {
            sf::Cursor cross_cursor;
            if(cross_cursor.loadFromSystem(sf::Cursor::Cross))
            {
                window.setMouseCursor(cross_cursor);
            }
        }
        else
        {
            sf::Cursor cross_cursor;
            if(cross_cursor.loadFromSystem(sf::Cursor::Arrow))
            {
                window.setMouseCursor(cross_cursor);
            }
        }

        // Create Overlay
        ImGui::SetNextWindowBgAlpha(0.35f);
        bool show_overlay = true;
        ImGui::Begin("Test", &show_overlay, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove);

        ImGui::Text("Mouse Position: (%.1f,%.1f)px", io.MousePos.x, io.MousePos.y);

        sf::Vector2f mouse_coordinates = window.mapPixelToCoords({(int)io.MousePos.x, (int)io.MousePos.y});
        ImGui::Text("Coordinates: (%.1f,%.1f)", mouse_coordinates.x, mouse_coordinates.y);

        ImGui::Text("Scalar: %.1f", scalar);

        ImGui::End();
        // End Overlay

        window.clear(sf::Color::White);

        window.draw(graph, t);
        ImGui::SFML::Render(window);

        window.display();
    }

    return 0;
}
