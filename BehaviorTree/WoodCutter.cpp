#include "Characters.h"

#include <iostream>

namespace simulation
{
WoodCutter::WoodCutter(bt::Blackboard& shared, TavernKeeper& tavernKeeper) : bt::Model(shared)
{
    Local().Set(keys::Wood, 0);
    Local().Set(keys::WorkLength, 0);
    Local().Set(keys::Stamina, 10);
    Local().Set(keys::Coin, 0);
    Local().Set(keys::Intoxication, 0);
    Local().Set(keys::SleepProgress, 0);
    using namespace bt;
    auto walk = std::make_unique<Action>([this] {
        if (Local().Get(keys::WorkLength) < 5)
        {
            std::cout << "나무꾼: 일터로 가는중~(" << Local().Get(keys::WorkLength) << "/5)\n";
            ++Local().Get(keys::WorkLength);
            return Status::Running;
        }
        return Status::Success;
    }, [] { std::cout << "나무꾼: 자, 오늘도 일하러 가볼까!!\n"; },
       [] { std::cout << "나무꾼: 일터 도착!\n"; });

    auto chop = std::make_unique<Action>([this] {
        if (Local().Get(keys::Stamina) > 0)
        {
            std::cout << "나무꾼: 아직 더할 수 있어 더 나무를 패자고!(" << Local().Get(keys::Stamina) << "/10)\n";
            --Local().Get(keys::Stamina);
            ++Local().Get(keys::Wood);
            return Status::Running;
        }
        return Status::Success;
    }, [] { std::cout << "나무꾼: 도끼질 시작!\n"; },
       [] { std::cout << "나무꾼: 이제 지쳤구먼... 마을에 팔러가야겠어\n"; });

    auto sell = std::make_unique<Action>([this] {
        if (Local().Get(keys::Wood) > 0)
        {
            if (Shared().Get(keys::SellerInToilet))
            {
                std::cout << "나무꾼: 상점주인이 돌아올 때까지 기다려야겠군.\n";
                return Status::Running;
            }
            std::cout << "나무꾼: 아직 더 남았어 더 사라고(" << Local().Get(keys::Wood) << "/10)\n";
            --Local().Get(keys::Wood);
            Local().Get(keys::Coin) += 5;
            return Status::Running;
        }
        return Status::Success;
    }, [this] {
        Shared().Set(keys::SellingWood, true);
        std::cout << "나무꾼: 이봐! 오늘 작업한 것 팔러왔어!\n";
    }, [this] {
        Shared().Set(keys::SellingWood, false);
        std::cout << "나무꾼: 이제 " << Local().Get(keys::Coin) << "개의 코인이 있어!\n";
    });

    auto visit = std::make_unique<Action>([] {
        std::cout << "나무꾼: 주인장, 술 한 잔 부탁하네!\n";
        return Status::Success;
    }, [this] {
        Shared().Set(keys::VisitingTavern, true);
        std::cout << "나무꾼: 나무도 다 팔았으니 주점에 들러야겠어!\n";
    });

    auto drink = std::make_unique<Action>([this, &tavernKeeper] {
        if (Local().Get(keys::Coin) < 5 || !tavernKeeper.ServeDrink(Local().Get(keys::Coin)))
        {
            std::cout << "나무꾼: 오늘은 더 마실 수 없군. 집에 가서 쉬어야겠어.\n";
            return Status::Success;
        }
        Local().Get(keys::Coin) -= 5;
        Local().Get(keys::Intoxication) += 5;
        std::cout << "나무꾼: 나무는 잘 팔았지! 술도 좋구먼. 남은 코인 " << Local().Get(keys::Coin)
            << ", 취기 " << Local().Get(keys::Intoxication) << "/20\n";
        return Local().Get(keys::Intoxication) >= 20 ? Status::Success : Status::Running;
    }, [this] {
        Shared().Set(keys::Drinking, true);
        std::cout << "나무꾼: 오늘 번 돈으로 목을 축여 볼까.\n";
    }, [this] {
        Shared().Set(keys::Drinking, false);
        Shared().Set(keys::VisitingTavern, false);
        std::cout << "나무꾼: 잘 마셨네. 이제 집에 가서 자야겠어.\n";
    });

    auto sleepAction = std::make_unique<Action>([this] {
        ++Local().Get(keys::SleepProgress);
        std::cout << "나무꾼: 쿨쿨.... " << Local().Get(keys::SleepProgress) << "/5\n";
        return Local().Get(keys::SleepProgress) >= 5 ? Status::Success : Status::Running;
    }, [this] {
        Local().Set(keys::SleepProgress, 0);
        std::cout << "나무꾼: 집에 도착했으니 푹 자자.\n";
    }, [this] {
        Local().Set(keys::Intoxication, 0);
        Local().Set(keys::Stamina, 10);
        Local().Set(keys::WorkLength, 0);
        std::cout << "나무꾼: 술도 깨고 기운도 다 돌아왔어!\n";
    });

    SetTree(std::make_unique<Repeat>(std::make_unique<Sequence>(ChildrenOf(
        std::move(walk), std::move(chop), std::move(sell),
        std::move(visit), std::move(drink), std::move(sleepAction)))));
}
}
