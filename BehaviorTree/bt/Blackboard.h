#pragma once

#include <any>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace bt
{
// Central key declarations associate each name with its expected value type.
template <typename T>
struct Key { const char* name; };

// Single-threaded storage. Missing keys/type mismatches are programming errors.
class Blackboard
{
public:
    template <typename T>
    void Set(Key<T> key, T value)
    {
        auto found = values_.find(key.name);
        if (found == values_.end())
        {
            values_.emplace(key.name, std::any(std::move(value)));
            return;
        }
        auto* stored = std::any_cast<T>(&found->second);
        if (!stored) throw std::logic_error(std::string("Blackboard type mismatch: ") + key.name);
        *stored = std::move(value);
    }

    template <typename T>
    [[nodiscard]] const T& Get(Key<T> key) const
    {
        auto found = values_.find(key.name);
        if (found == values_.end())
            throw std::out_of_range(std::string("Missing blackboard key: ") + key.name);
        const auto* stored = std::any_cast<T>(&found->second);
        if (!stored) throw std::logic_error(std::string("Blackboard type mismatch: ") + key.name);
        return *stored;
    }

    // Mutable access supports ++, -- and += directly on blackboard values.
    template <typename T>
    [[nodiscard]] T& Get(Key<T> key)
    {
        return const_cast<T&>(std::as_const(*this).Get(key));
    }

private:
    std::unordered_map<std::string, std::any> values_;
};
}
