#ifndef UNLOGIC_ERROR_H
#define UNLOGIC_ERROR_H

namespace unlogic
{
    class Error
    {
        char const *error_;

    public:
        Error(char const *error) : error_(error) {}
    };
}

#endif //UNLOGIC_ERROR_H
