#ifndef UNLOGIC_IO_H
#define UNLOGIC_IO_H

#include <string>
#include <filesystem>

namespace unlogic
{
    std::string load_file(std::filesystem::path const &path);
}

#endif //UNLOGIC_IO_H
