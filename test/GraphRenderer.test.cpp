#include <gtest/gtest.h>
#include "Parser.h"
#include "GraphRenderer.test.h"

TEST(GraphRenderer, QuadraticImageGeneration)
{
    // Generate image
    sf::RenderTexture texture;
    texture.create(250, 250);

    GraphRenderer renderer("f[x] = x^2; f(x);");

    renderer.Draw(texture);

    sf::Image image = texture.getTexture().copyToImage();
    std::vector<sf::Uint8> buffer;
    image.saveToMemory(buffer, "jpg");
}