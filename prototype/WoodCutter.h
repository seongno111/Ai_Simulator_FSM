#pragma once

#include "../fsm/AIModel.h"

namespace Seller
{
    class Context;
}

namespace WoodCutter
{
// Replace this neutral model with a scenario character later.
// Update and the typed transition functions are inherited from AIModel.
class Context final : public fsm::AIModel<Context>
{
public:
    explicit Context(Seller::Context& seller);
    // Non-owning reference: the seller must outlive this woodcutter.
    Seller::Context& seller;
    // Add character data or a reference to the scenario world here later.
    int wood = 0;
    int work_length = 0;
    int stamina = 10;
    int coin = 0;
};

class Go_To_Work final : public fsm::State<Context>
{
public:
    [[nodiscard]] std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};

class WoodCutting final : public fsm::State<Context>
{
public:
    [[nodiscard]] std::string_view Name() const noexcept override;
    void Enter(Context& owner) noexcept override;
    void Execute(Context& owner) noexcept override;
    void Exit(Context& owner) noexcept override;
};

class Go_To_Sell final : public fsm::State<Context>
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
