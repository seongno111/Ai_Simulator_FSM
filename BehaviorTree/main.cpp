#include "Characters.h"

#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
    // All narrow string literals in this project are compiled as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
#endif
    bt::Blackboard shared;
    shared.Set(simulation::keys::SellingWood, false);
    shared.Set(simulation::keys::VisitingTavern, false);
    shared.Set(simulation::keys::Drinking, false);
    shared.Set(simulation::keys::SellerInToilet, false);
    simulation::Seller seller(shared);
    simulation::TavernKeeper tavernKeeper(shared);
    simulation::WoodCutter woodCutter(shared, tavernKeeper);

    for (int tick = 1; tick <= 100; ++tick)
    {
        std::cout << "\n--- Tick " << tick << " ---\n";
        if (woodCutter.Update() == bt::Status::Failure
            || seller.Update() == bt::Status::Failure
            || tavernKeeper.Update() == bt::Status::Failure)
        {
            std::cerr << "Behavior tree failed.\n";
            return 1;
        }
    }
}
