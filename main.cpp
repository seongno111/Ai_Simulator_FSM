#include "prototype/WoodCutter.h"
#include "Seller.h"

#include <cstdio>

int main()
{
    // The model constructor creates all states and enters the initial state.
    // Create the seller first so it outlives the woodcutter's reference.
    Seller::Context seller;
    WoodCutter::Context woodCutter(seller);

    // These manual steps illustrate the API, without choosing a scenario.
    for (int i = 0; i < 40; i++) {
        (void)woodCutter.Update();
        (void)seller.Update();
    }

    return 0;
}
