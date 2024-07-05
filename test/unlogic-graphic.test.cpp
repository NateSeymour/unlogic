//
// Created by nathan on 6/11/24.
//
#include <gtest/gtest.h>
#include <string>
#include "Compiler.h"
#include "graphic/Graph.h"

TEST(UnlogicGraphic, PolynomialRender100x100)
{
    sf::RenderTexture texture;
    texture.create(100, 100);

    unlogic::Graph graph("f(x) := x^2");

    texture.DrawOnto(graph);

    sf::Image image = texture.getTexture().copyToImage();
    std::vector<sf::Uint8> buffer;
    image.saveToMemory(buffer, "png");
}

TEST(UnlogicGraphic, PolynomialRender1000x1000)
{
    sf::RenderTexture texture;
    texture.create(1000, 1000);

    unlogic::Graph graph("f(x) := x^2");

    texture.DrawOnto(graph);

    sf::Image image = texture.getTexture().copyToImage();
    std::vector<sf::Uint8> buffer;
    image.saveToMemory(buffer, "png");
}

TEST(UnlogicGraphic, ExponentialRender100x100)
{
    sf::RenderTexture texture;
    texture.create(100, 100);

    unlogic::Graph graph("f(x) := 2^x");

    texture.DrawOnto(graph);

    sf::Image image = texture.getTexture().copyToImage();
    std::vector<sf::Uint8> buffer;
    image.saveToMemory(buffer, "png");
}