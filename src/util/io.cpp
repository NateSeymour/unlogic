#include <fstream>
#include "io.h"

std::string unlogic::load_file(std::filesystem::path const &path)
{
    std::ifstream fp(path, std::ios::in | std::ios::binary);
    return {std::istreambuf_iterator<char>{fp}, std::istreambuf_iterator<char>{}};
}