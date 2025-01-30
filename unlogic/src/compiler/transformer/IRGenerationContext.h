#ifndef UNLOGIC_IRGENERATIONCONTEXT_H
#define UNLOGIC_IRGENERATIONCONTEXT_H

#include <city/Value.h>
#include <map>
#include <optional>
#include <ranges>
#include <vector>

namespace unlogic
{
    class Scope
    {
        std::vector<std::map<std::string, city::Value *>> layers;

    public:
        std::optional<city::Value *> Lookup(std::string const &key)
        {
            for (auto &layer: this->layers | std::ranges::views::reverse)
            {
                if (layer.contains(key))
                    return layer[key];
            }

            return std::nullopt;
        }

        void Insert(std::string const &key, city::Value *value)
        {
            this->layers.back()[key] = value;
        }

        void PushLayer()
        {
            this->layers.emplace_back();
        }

        void PopLayer()
        {
            this->layers.pop_back();
        }
    };

    struct IRGenerationContext
    {
        city::IRModule &module;
        Scope &scope;
    };
} // namespace unlogic

#endif // UNLOGIC_IRGENERATIONCONTEXT_H
