#pragma once
#pragma once

#include "fsm/AIModel.h"

namespace Seller
{
    // Replace this neutral model with a scenario character later.
    // Update and the typed transition functions are inherited from AIModel.
    class Context final : public fsm::AIModel<Context>
    {
    public:
        Context();
        // Add character data or a reference to the scenario world here later.
        int sleep = 0;
        int cigarette = 0;
    };

    class Open_for_business final : public fsm::State<Context>
    {
    public:
        [[nodiscard]] std::string_view Name() const noexcept override;
        void Enter(Context& owner) noexcept override;
        void Execute(Context& owner) noexcept override;
        void Exit(Context& owner) noexcept override;
    };

    class Working final : public fsm::State<Context>
    {
    public:
        [[nodiscard]] std::string_view Name() const noexcept override;
        void Enter(Context& owner) noexcept override;
        void Execute(Context& owner) noexcept override;
        void Exit(Context& owner) noexcept override;
    };

    class Buying final : public fsm::State<Context>
    {
    public:
        [[nodiscard]] std::string_view Name() const noexcept override;
        void Enter(Context& owner) noexcept override;
        void Execute(Context& owner) noexcept override;
        void Exit(Context& owner) noexcept override;
    };

    class Sleep final : public fsm::State<Context>
    {
    public:
        [[nodiscard]] std::string_view Name() const noexcept override;
        void Enter(Context& owner) noexcept override;
        void Execute(Context& owner) noexcept override;
        void Exit(Context& owner) noexcept override;
    };

    class Cigarette final : public fsm::State<Context>
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
