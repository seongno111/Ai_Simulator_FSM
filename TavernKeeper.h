#pragma once

#include "fsm/AIModel.h"

namespace WoodCutter { class Context; }

namespace TavernKeeper
{
class Context final : public fsm::AIModel<Context>
{
public:
    Context();

    // One customer at a time. The caller ends the visit before going to sleep.
    void BeginVisit(WoodCutter::Context& visitor) noexcept;
    void EndVisit(WoodCutter::Context& visitor) noexcept;
    [[nodiscard]] bool ServeDrink(WoodCutter::Context& visitor) noexcept;

    WoodCutter::Context* customer = nullptr;
    int dishProgress = 0;
    int cleaningProgress = 0;
};

class WashingDishes final : public fsm::State<Context>
{
public:
    std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};

class Cleaning final : public fsm::State<Context>
{
public:
    std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};

class ServingCustomer final : public fsm::State<Context>
{
public:
    std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};
}
