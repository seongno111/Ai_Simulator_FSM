#include "prototype/Prototype.h"

#include <cstdio>

int main()
{
    std::puts("FSM prototype: A -> B -> previous (A)");

    // The model constructor creates all states and enters the initial state.
    prototype::Context model;

    // These manual steps illustrate the API, without choosing a scenario.
    for (int i = 0; i < 40; i++) {
        model.Update();
    }

    return 0;
}
