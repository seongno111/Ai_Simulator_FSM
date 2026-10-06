#include "prototype/Prototype.h"

#include <cstdio>

int main()
{
    // Declare states first so they remain alive for the context's lifetime.
    prototype::StateA stateA;
    prototype::StateB stateB;
    prototype::GlobalState global;
    prototype::Context context;

    std::puts("FSM prototype: A -> B -> previous (A)");

    // These manual steps illustrate the API, without choosing a scenario.
    if (!context.machine.SetGlobalState(&global)
        || !context.machine.Start(stateA)
        || !context.machine.Update()
        || !context.machine.ChangeState(stateB)
        || !context.machine.Update()
        || !context.machine.RevertToPreviousState()
        || !context.machine.Update())
    {
        std::fputs("FSM prototype: an operation was rejected.\n", stderr);
        return 1;
    }

    return 0;
}
