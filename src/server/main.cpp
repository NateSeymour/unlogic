#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <cryptopp/base64.h>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "../graphic/Graph.h"

using namespace Pistache;
using namespace nlohmann;

class UnlogicServer
{
public:
    // Routes
    /**
     *
     * @param req - JSON Request
     * {
     *      "f": OPTIONAL("f(x) = x^2") - function of which to produce a graph
     * }
     * @param res
     */
    void GetUnlogic(Rest::Request const &req, Http::ResponseWriter res)
    {
        std::string b64enc = req.param(":b64enc").as<std::string>();
        std::string decoded;

        // Decode parameters
        CryptoPP::Base64Decoder decoder;
        decoder.Put((CryptoPP::byte*)b64enc.data(), b64enc.size());
        decoder.MessageEnd();

        CryptoPP::word64 size = decoder.MaxRetrievable();
        if(size && size <= SIZE_MAX)
        {
            decoded.resize(size);
            decoder.Get((CryptoPP::byte*)decoded.data(), decoded.size());
        }

        json parameters = json::parse(decoded);

        // Generate image
        sf::RenderTexture texture;
        texture.create(1000, 1000);
        texture.clear(sf::Color::White);

        unlogic::Graph graph(parameters.value("f", "f(x) = x^2"));

        texture.draw(graph);
        texture.display();

        sf::Image image = texture.getTexture().copyToImage();
        std::vector<sf::Uint8> buffer;
        image.saveToMemory(buffer, "jpg");

        // Respond
        res.send(Http::Code::Ok, (char const *)buffer.data(), buffer.size(), MIME(Image, Png));
    }

    UnlogicServer() = default;
};

int main()
{
    unlogic::Compiler::InitializeCompilerRuntime();

    Address address(Ipv4::any(), Port(9000));

    Rest::Router router;
    UnlogicServer server;

    Rest::Routes::Get(router, "/unlogic/:b64enc", Rest::Routes::bind(&UnlogicServer::GetUnlogic, &server));

    Http::Endpoint endpoint(address);
    endpoint.init();
    endpoint.setHandler(router.handler());
    endpoint.serve();

    endpoint.shutdown();
}