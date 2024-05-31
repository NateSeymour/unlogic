#include <benchmark/benchmark.h>
#include "Graph.h"

static void BM_QuadraticImageGeneration(benchmark::State &state)
{
    for(auto _ : state)
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
}
BENCHMARK(BM_QuadraticImageGeneration);