#pragma once

#include "../fsm/StateMachine.h"

namespace prototype
{
// Integration placeholder only: this is not a scenario character.
struct Context final
{
    Context() noexcept : machine(*this) {}

    // Add character data or a reference to the scenario world here later.
    fsm::StateMachine<Context> machine;
};

class StateA final : public fsm::State<Context>
{
public:
    [[nodiscard]] std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};

class StateB final : public fsm::State<Context>
{
public:
    [[nodiscard]] std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};

// Optional hook for checks that are common to every current state.
class GlobalState final : public fsm::State<Context>
{
public:
    [[nodiscard]] std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};
}
