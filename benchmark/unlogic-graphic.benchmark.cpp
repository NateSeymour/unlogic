//
// Created by nathan on 6/11/24.
//
#include <benchmark/benchmark.h>
#include "Compiler.h"
#include "graphic/Graph.h"

// TODO: Replace the native functions of SFML with a better PNG library, because stb is *slow as hell*

static void BM_PolynomialRender100x100(benchmark::State &state)
{
    unlogic::Compiler::InitializeCompilerRuntime();
    sf::RenderTexture texture;
    texture.create(100, 100);
    std::vector<sf::Uint8> buffer;

    for (auto _: state)
    {
        texture.clear();

        unlogic::Graph graph("f(x) = x^2");

        texture.draw(graph);

        sf::Image image = texture.getTexture().copyToImage();
        image.saveToMemory(buffer, "png");
    }
}
BENCHMARK(BM_PolynomialRender100x100)->Iterations(100)->Unit(benchmark::kMillisecond);

static void BM_PolynomialRender1000x1000(benchmark::State &state)
{
    unlogic::Compiler::InitializeCompilerRuntime();
    sf::RenderTexture texture;
    texture.create(1000, 1000);
    std::vector<sf::Uint8> buffer;

    for (auto _: state)
    {
        texture.clear();

        unlogic::Graph graph("f(x) = x^2");

        texture.draw(graph);

        sf::Image image = texture.getTexture().copyToImage();
        image.saveToMemory(buffer, "png");
    }
}
BENCHMARK(BM_PolynomialRender1000x1000)->Iterations(100)->Unit(benchmark::kMillisecond);

static void BM_PolynomialRender10Functions1000x1000(benchmark::State &state)
{
    unlogic::Compiler::InitializeCompilerRuntime();
    sf::RenderTexture texture;
    texture.create(1000, 1000);
    std::vector<sf::Uint8> buffer;

    for (auto _: state)
    {
        texture.clear();

        unlogic::Graph graph({
            "a(x) = x^2",
            "b(x) = x^3",
            "c(x) = x^4",
            "d(x) = x^2 - 2*x + 4",
            "e(x) = 314*12+x",
            "f(x) = 2^x",
            "g(x) = 900 / 4*x",
            "h(x) = x^1/2",
            "i(x) = 2*x + 15",
            "j(x) = 300*x",
        });

        texture.draw(graph);

        sf::Image image = texture.getTexture().copyToImage();
        image.saveToMemory(buffer, "png");
    }
}
BENCHMARK(BM_PolynomialRender10Functions1000x1000)->Iterations(100)->Unit(benchmark::kMillisecond);