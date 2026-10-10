#pragma once

#include "bt/BehaviorTree.h"
#include "BlackboardKeys.h"

namespace simulation
{
class Seller final : public bt::Model
{
public:
    explicit Seller(bt::Blackboard& shared);
    [[nodiscard]] bt::Status Update() override;
};

class TavernKeeper final : public bt::Model
{
public:
    explicit TavernKeeper(bt::Blackboard& shared);
    [[nodiscard]] bool ServeDrink(int customerCoins);
};

class WoodCutter final : public bt::Model
{
public:
    WoodCutter(bt::Blackboard& shared, TavernKeeper& tavernKeeper);
};
}
