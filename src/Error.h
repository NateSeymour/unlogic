#ifndef UNLOGIC_ERROR_H
#define UNLOGIC_ERROR_H

#include <string>

namespace unlogic
{
    struct Error
    {
        std::string message;

        Error(std::string message) : message(std::move(message)) {}
    };
} // namespace unlogic

#endif // UNLOGIC_ERROR_H
