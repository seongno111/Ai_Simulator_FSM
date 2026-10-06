#include "Prototype.h"

#include <cstdio>

namespace prototype
{
std::string_view StateA::Name() const noexcept { return "StateA"; }
void StateA::Enter(Context&) noexcept { std::puts("StateA.Enter"); }
void StateA::Execute(Context&) noexcept
{
    std::puts("StateA.Execute");
    // TODO: Add behavior and transition conditions after defining the scenario.
}
void StateA::Exit(Context&) noexcept { std::puts("StateA.Exit"); }

std::string_view StateB::Name() const noexcept { return "StateB"; }
void StateB::Enter(Context&) noexcept { std::puts("StateB.Enter"); }
void StateB::Execute(Context&) noexcept
{
    std::puts("StateB.Execute");
    // TODO: Add behavior and transition conditions after defining the scenario.
}
void StateB::Exit(Context&) noexcept { std::puts("StateB.Exit"); }

std::string_view GlobalState::Name() const noexcept { return "Global"; }
void GlobalState::Enter(Context&) noexcept {}
void GlobalState::Execute(Context&) noexcept
{
    std::puts("Global.Execute");
    // TODO: Add shared checks here if the scenario needs them.
}
void GlobalState::Exit(Context&) noexcept {}
}
