#pragma once

#include "State.h"

namespace fsm
{
// Non-owning, single-threaded FSM. Owner and states must outlive this object.
template <typename Owner>
class StateMachine final
{
public:
    explicit StateMachine(Owner& owner) noexcept : owner_(owner) {}

    StateMachine(const StateMachine&) = delete;
    StateMachine& operator=(const StateMachine&) = delete;
    StateMachine(StateMachine&&) = delete;
    StateMachine& operator=(StateMachine&&) = delete;

    // Starts only once. The initial state's Enter is called immediately.
    [[nodiscard]] bool Start(State<Owner>& initialState) noexcept
    {
        if (current_ != nullptr || updating_ || transitioning_)
        {
            return false;
        }
        return ChangeState(initialState);
    }

    // A global state is an Execute-only hook, configured before Start.
    [[nodiscard]] bool SetGlobalState(State<Owner>* state) noexcept
    {
        if (current_ != nullptr || updating_ || transitioning_)
        {
            return false;
        }
        global_ = state;
        return true;
    }

    // One tick: global Execute, then the current state's Execute.
    [[nodiscard]] bool Update() noexcept
    {
        if (current_ == nullptr || updating_ || transitioning_)
        {
            return false;
        }

        updating_ = true;
        if (global_ != nullptr)
        {
            global_->Execute(owner_);
        }
        current_->Execute(owner_);
        updating_ = false;
        return true;
    }

    // Immediate transition; allowed in Execute, rejected in Enter/Exit.
    // Calling this before Start also enters the first state.
    [[nodiscard]] bool ChangeState(State<Owner>& nextState) noexcept
    {
        if (transitioning_ || current_ == &nextState)
        {
            return false;
        }

        transitioning_ = true;
        if (current_ != nullptr)
        {
            current_->Exit(owner_);
        }

        previous_ = current_;
        current_ = &nextState;
        current_->Enter(owner_);
        transitioning_ = false;
        return true;
    }

    [[nodiscard]] bool RevertToPreviousState() noexcept
    {
        return previous_ != nullptr && ChangeState(*previous_);
    }

    [[nodiscard]] bool IsInState(const State<Owner>& state) const noexcept
    {
        return current_ == &state;
    }

    [[nodiscard]] const State<Owner>* CurrentState() const noexcept
    {
        return current_;
    }

    [[nodiscard]] const State<Owner>* PreviousState() const noexcept
    {
        return previous_;
    }

    [[nodiscard]] const State<Owner>* GlobalState() const noexcept
    {
        return global_;
    }

private:
    Owner& owner_;
    State<Owner>* current_ = nullptr;
    State<Owner>* previous_ = nullptr;
    State<Owner>* global_ = nullptr;
    bool updating_ = false;
    bool transitioning_ = false;
};
}
