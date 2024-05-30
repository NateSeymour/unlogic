#ifndef UNLOGIC_PROGRAM_H
#define UNLOGIC_PROGRAM_H

#include "Node.h"
#include "Parser.h"

namespace unlogic
{
    class Program
    {
        EvaluationContext ctx_;
        std::unique_ptr<Node> tree_;

    public:
        Program()
        {
            this->tree_ = std::make_unique<EmptyNode>();
        }

        explicit Program(std::string const &initial)
        {
            this->tree_ = unlogic::parse(initial);
        }
    };

    Function compile(std::string const& function);
}

#endif //UNLOGIC_PROGRAM_H
