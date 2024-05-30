#ifndef UNLOGIC_PROGRAM_H
#define UNLOGIC_PROGRAM_H

#include "Node.h"
#include "Parser.h"

namespace unlogic
{
    class Program
    {
        unlogic::EvaluationContext ctx_;
        std::unique_ptr<unlogic::Node> tree_;

    public:
        double Evaluate(std::string const &input);

        Program()
        {
            this->tree_ = std::make_unique<unlogic::EmptyNode>();
        }

        explicit Program(std::string const &initial)
        {
            this->tree_ = unlogic::parse(initial);
        }
    };
}

#endif //UNLOGIC_PROGRAM_H
