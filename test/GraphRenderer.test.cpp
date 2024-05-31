#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include "Graph.h"

TEST(GraphRenderer, QuadraticImageGeneration)
{
    // Generate image
    Graph graph({"f[x] = x^2;"});

    sf::RenderTexture texture;
    texture.create(250, 250);

    texture.clear(sf::Color::White);
    texture.draw(graph);

    sf::Image image = texture.getTexture().copyToImage();
    std::vector<sf::Uint8> buffer;
    image.saveToMemory(buffer, "png");
}