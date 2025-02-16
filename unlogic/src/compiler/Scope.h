#ifndef UNLOGIC_PARENT_SCOPE_H
#define UNLOGIC_PARENT_SCOPE_H

#include <city/value/Value.h>
#include <optional>
#include <ranges>
#include <unordered_map>
#include <vector>

namespace unlogic
{
    class Scope
    {
        std::vector<std::unordered_map<std::string, city::Value *> > layers;

    public:
        std::optional<city::Value *> Lookup(std::string const &key)
        {
            for (auto &layer: this->layers | std::ranges::views::reverse)
            {
                if (layer.contains(key))
                {
                    return layer[key];
                }
            }

            return std::nullopt;
        }

        void Set(std::string const &key, city::Value *value)
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
}; // namespace unlogic

#endif // UNLOGIC_PARENT_SCOPE_H
