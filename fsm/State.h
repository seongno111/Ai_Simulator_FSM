#pragma once

#include <string_view>

namespace fsm
{
// Owner supplies the data and behavior used by each state.
template <typename Owner>
class State
{
public:
    virtual ~State() = default;

    [[nodiscard]] virtual std::string_view Name() const noexcept = 0;
    virtual void Enter(Owner& owner) noexcept = 0;
    virtual void Execute(Owner& owner) noexcept = 0;
    virtual void Exit(Owner& owner) noexcept = 0;
};
}
