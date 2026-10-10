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
        void BeginBuying() noexcept;
        void EndBuying() noexcept;
        [[nodiscard]] bool CanBuyWood() const noexcept;
        // Add character data or a reference to the scenario world here later.
        int sleep = 0;
        int cigarette = 0;
        int toilet = 0;
        int toiletProgress = 0;
        bool buyingRequested = false;
        bool tradeChangedDuringToilet = false;
        bool resumingFromToilet = false;
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

    class Toilet final : public fsm::State<Context>
    {
    public:
        std::string_view Name() const noexcept override;
        void Enter(Context& owner) noexcept override;
        void Execute(Context& owner) noexcept override;
        void Exit(Context& owner) noexcept override;
    };

    // Checks urgent needs before the current state's Execute.
    class GlobalState final : public fsm::State<Context>
    {
    public:
        [[nodiscard]] std::string_view Name() const noexcept override;
        void Enter(Context& owner) noexcept override;
        void Execute(Context& owner) noexcept override;
        void Exit(Context& owner) noexcept override;
    };
}
