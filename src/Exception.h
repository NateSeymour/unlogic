#ifndef UNLOGIC_EXCEPTION_H
#define UNLOGIC_EXCEPTION_H

#include <stdexcept>
#include "TokenViewer.h"

namespace unlogic
{
    class TokenException : public std::exception
    {
        TokenViewer const &tv_;

    public:
        enum TokenExceptionType
        {
            UnknownSymbol
        };

        TokenExceptionType type;

        char const *what() const noexcept override
        {
            return "an issue occurred while tokenizing the input.";
        }

        TokenException(TokenViewer const &tv, TokenExceptionType type = UnknownSymbol) : tv_(tv), type(type) {}
    };
}

#endif //UNLOGIC_EXCEPTION_H
