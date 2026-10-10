#include "Characters.h"

#include <iostream>

namespace simulation
{
Seller::Seller(bt::Blackboard& shared) : bt::Model(shared)
{
    Local().Set(keys::Sleep, 0);
    Local().Set(keys::Cigarette, 0);
    Local().Set(keys::Toilet, 0);
    Local().Set(keys::ToiletProgress, 0);
    using namespace bt;
    auto open = std::make_unique<Action>([] { return Status::Success; },
        [] { std::cout << "상점주인: 어후, 오늘도 날이 밝았구먼. 가게문이나 열어 둬야지...\n"; },
        [] { std::cout << "상점주인: 그래, 오늘도 일해야지.\n"; });

    auto work = std::make_unique<Action>([this] {
        if (Local().Get(keys::Sleep) > 10 || Local().Get(keys::Cigarette) > 5) return Status::Success;
        std::cout << "상점주인: 가게 보는 중...." << Local().Get(keys::Sleep) << ", " << Local().Get(keys::Cigarette) << '\n';
        ++Local().Get(keys::Sleep);
        ++Local().Get(keys::Cigarette);
        return Status::Running;
    }, [] { std::cout << "상점주인: 영업 시작.\n"; });

    auto nap = std::make_unique<Action>([this] {
        if (Local().Get(keys::Sleep) > 0)
        {
            std::cout << "상점주인: ZZZZZZ\n";
            --Local().Get(keys::Sleep);
            return Status::Running;
        }
        return Status::Success;
    }, [] { std::cout << "상점주인: 하~암, 어후 졸려 한숨 자야겠군....\n"; },
       [] { std::cout << "상점주인: 어으 잘잤다.\n"; });

    auto smoke = std::make_unique<Action>([this] {
        if (Local().Get(keys::Cigarette) > 0)
        {
            std::cout << "상점주인: 스읍, 후우~" << Local().Get(keys::Cigarette) << "/5\n";
            --Local().Get(keys::Cigarette);
            return Status::Running;
        }
        return Status::Success;
    }, [this] {
        Local().Set(keys::Cigarette, 5);
        std::cout << "상점주인: 담배나 한대 태우고 와야겠어.\n";
    }, [] { std::cout << "상점주인: 다시 돌아가야지...\n"; });

    auto rest = std::make_unique<Selector>(ChildrenOf(
        std::make_unique<Sequence>(ChildrenOf(
            std::make_unique<Condition>([this] { return Local().Get(keys::Sleep) > 10; }), std::move(nap))),
        std::make_unique<Sequence>(ChildrenOf(
            std::make_unique<Condition>([this] { return Local().Get(keys::Cigarette) > 5; }), std::move(smoke)))));
    auto routine = std::make_unique<Repeat>(std::make_unique<Sequence>(
        ChildrenOf(std::move(work), std::move(rest))));
    auto buy = std::make_unique<Action>([] {
        std::cout << "상점주인: 나무를 사는 중....\n";
        return Status::Running;
    }, [] { std::cout << "상점주인: 가져온 나무를 한번 볼까?\n"; },
       [] { std::cout << "상점주인: 거래 끝! 다시 가게를 봐야지.\n"; });

    // Abort the interrupted routine: after buying, start again at Work.
    auto normal = std::make_unique<Sequence>(ChildrenOf(std::move(open),
        std::make_unique<PrioritySelector>([this] { return Shared().Get(keys::SellingWood); },
            std::move(buy), std::move(routine), false)));

    auto toiletAction = std::make_unique<Action>([this] {
        ++Local().Get(keys::ToiletProgress);
        std::cout << "상점주인: 화장실 이용 중.... " << Local().Get(keys::ToiletProgress) << "/5\n";
        return Local().Get(keys::ToiletProgress) >= 5 ? Status::Success : Status::Running;
    }, [this] {
        Local().Set(keys::ToiletProgress, 0);
        Shared().Set(keys::SellerInToilet, true);
        std::cout << "상점주인: 급하군! 하던 일은 멈추고 화장실부터 가야겠어.\n";
    }, [this] {
        Local().Set(keys::Toilet, 0);
        Shared().Set(keys::SellerInToilet, false);
        std::cout << "상점주인: 이제 괜찮군. 하던 일을 이어가자.\n";
    });

    // Highest priority, including opening, buying, sleep and smoking.
    // Suspend the whole normal tree so rest progress is preserved.
    SetTree(std::make_unique<PrioritySelector>(
        [this] { return Local().Get(keys::Toilet) >= 10; },
        std::move(toiletAction), std::move(normal), true));
}

bt::Status Seller::Update()
{
    // Shared update before traversal: unlike an Action, this runs in every branch.
    if (!Shared().Get(keys::SellerInToilet))
        ++Local().Get(keys::Toilet);
    return bt::Model::Update();
}
}
