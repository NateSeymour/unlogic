#include <gtest/gtest.h>
#include "Graph.h"

TEST(GraphRenderer, QuadraticImageGeneration)
{
    // Generate image
    sf::RenderTexture texture;
    texture.create(250, 250);

    GraphRenderer renderer("f[x] = x^2;");

    renderer.Draw(texture);

    sf::Image image = texture.getTexture().copyToImage();
    std::vector<sf::Uint8> buffer;
    image.saveToMemory(buffer, "jpg");
}