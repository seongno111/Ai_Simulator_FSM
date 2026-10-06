#pragma once

#include "StateMachine.h"

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace fsm
{
// Derive as: class Character : public AIModel<Character>.
// Each model owns its registered states and its state machine.
template <typename Owner>
class AIModel
{
public:
    virtual ~AIModel() = default;

    AIModel(const AIModel&) = delete;
    AIModel& operator=(const AIModel&) = delete;
    AIModel(AIModel&&) = delete;
    AIModel& operator=(AIModel&&) = delete;

    // Models inherit this implementation, or override it to update their data.
    [[nodiscard]] virtual bool Update() noexcept
    {
        return machine_.Update();
    }

    template <typename StateType>
    [[nodiscard]] bool ChangeState() noexcept
    {
        auto* state = FindState<StateType>();
        return state != nullptr && machine_.ChangeState(*state);
    }

    [[nodiscard]] bool RevertToPreviousState() noexcept
    {
        return machine_.RevertToPreviousState();
    }

    template <typename StateType>
    [[nodiscard]] bool IsInState() const noexcept
    {
        const auto* state = FindState<StateType>();
        return state != nullptr && machine_.IsInState(*state);
    }

    [[nodiscard]] const StateMachine<Owner>& GetFSM() const noexcept
    {
        return machine_;
    }

protected:
    explicit AIModel(Owner& owner) noexcept : machine_(owner) {}

    // Call from the concrete model constructor, before starting the FSM.
    // One instance per concrete state type, per model.
    template <typename StateType, typename... Args>
    void RegisterState(Args&&... args)
    {
        static_assert(std::is_base_of_v<State<Owner>, StateType>,
            "StateType must derive from State<Owner>.");

        if (machine_.CurrentState() != nullptr || FindState<StateType>() != nullptr)
        {
            throw std::logic_error("Register states once per type, before Start.");
        }
        states_.push_back(std::make_unique<StateType>(std::forward<Args>(args)...));
    }

    template <typename StateType>
    [[nodiscard]] bool Start() noexcept
    {
        auto* state = FindState<StateType>();
        return state != nullptr && machine_.Start(*state);
    }

    template <typename StateType>
    [[nodiscard]] bool SetGlobalState() noexcept
    {
        auto* state = FindState<StateType>();
        return state != nullptr && machine_.SetGlobalState(state);
    }

private:
    template <typename StateType>
    [[nodiscard]] State<Owner>* FindState() const noexcept
    {
        static_assert(std::is_base_of_v<State<Owner>, StateType>,
            "StateType must derive from State<Owner>.");

        for (const auto& state : states_)
        {
            if (typeid(*state) == typeid(StateType))
            {
                return state.get();
            }
        }
        return nullptr;
    }

    // The machine is destroyed first; state addresses remain stable even if
    // the vector grows because each state is allocated separately.
    std::vector<std::unique_ptr<State<Owner>>> states_;
    StateMachine<Owner> machine_;
};
}
