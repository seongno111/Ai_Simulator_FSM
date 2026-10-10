#include "Characters.h"

#include <iostream>

namespace simulation
{
TavernKeeper::TavernKeeper(bt::Blackboard& shared) : bt::Model(shared)
{
    Local().Set(keys::DishProgress, 0);
    Local().Set(keys::CleaningProgress, 0);
    using namespace bt;
    auto wash = std::make_unique<Action>([this] {
        ++Local().Get(keys::DishProgress);
        std::cout << "주점 주인: 접시를 닦는 중. " << Local().Get(keys::DishProgress) << "/5\n";
        if (Local().Get(keys::DishProgress) < 5) return Status::Running;
        Local().Set(keys::DishProgress, 0);
        return Status::Success;
    }, [this] { std::cout << "주점 주인: 설거지를 하자. 현재 진행도 " << Local().Get(keys::DishProgress) << "/5\n"; });

    auto clean = std::make_unique<Action>([this] {
        ++Local().Get(keys::CleaningProgress);
        std::cout << "주점 주인: 바닥을 쓸고 닦는 중. " << Local().Get(keys::CleaningProgress) << "/10\n";
        if (Local().Get(keys::CleaningProgress) < 10) return Status::Running;
        Local().Set(keys::CleaningProgress, 0);
        return Status::Success;
    }, [this] { std::cout << "주점 주인: 주점을 청소하자. 현재 진행도 " << Local().Get(keys::CleaningProgress) << "/10\n"; });

    auto serve = std::make_unique<Action>([this] {
        if (Shared().Get(keys::Drinking))
            std::cout << "주점 주인: 오늘 나무는 잘 팔았는가?\n";
        else
            std::cout << "주점 주인: 여기 앉게. 술을 준비해 두겠네.\n";
        return Status::Running;
    }, [] { std::cout << "주점 주인: 어서 오게! 하던 일은 잠시 멈추지.\n"; },
       [] { std::cout << "주점 주인: 잘 가게! 하던 일을 마저 해야겠군.\n"; });

    // Suspend the fallback: its Sequence cursor and action progress survive.
    SetTree(std::make_unique<PrioritySelector>([this] { return Shared().Get(keys::VisitingTavern); },
        std::move(serve), std::make_unique<Repeat>(std::make_unique<Sequence>(
            ChildrenOf(std::move(wash), std::move(clean)))), true));
}

bool TavernKeeper::ServeDrink(int customerCoins)
{
    if (!Shared().Get(keys::VisitingTavern) || !Shared().Get(keys::Drinking) || customerCoins < 5) return false;
    std::cout << "주점 주인: 술 한 잔에 5코인이오. 오늘도 고생 많았네!\n";
    return true;
}
}
