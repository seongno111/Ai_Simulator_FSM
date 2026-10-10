#include "prototype/WoodCutter.h"
#include "Seller.h"
#include "TavernKeeper.h"

#include <cstdio>

int main()
{
    // The model constructor creates all states and enters the initial state.
    // Both shopkeepers must outlive the woodcutter's references.
    Seller::Context seller;
    TavernKeeper::Context tavernKeeper;
    WoodCutter::Context woodCutter(seller, tavernKeeper);

    // Each loop is one tick. Run long enough to observe repeated visits.
    for (int i = 0; i < 100; i++) {
        std::printf("\n--- Tick %d ---\n", i + 1);
        (void)woodCutter.Update();
        (void)seller.Update();
        (void)tavernKeeper.Update();
    }

    return 0;
}
