#include "WoodCutter.h"
#include "../Seller.h"
#include "../TavernKeeper.h"
#include <iostream>
#include <cstdio>

namespace WoodCutter
{
Context::Context(Seller::Context& seller, TavernKeeper::Context& tavernKeeper)
    : fsm::AIModel<Context>(*this), seller(seller), tavernKeeper(tavernKeeper)
{
    // Declare every state used by this model here. AIModel owns the instances.
    RegisterState<Go_To_Work>();
    RegisterState<WoodCutting>();
    RegisterState<Go_To_Sell>();
    RegisterState<VisitTavern>();
    RegisterState<Drinking>();
    RegisterState<Sleeping>();

    RegisterState<GlobalState>();

    // Start last: Enter may access this model and any of its registered states.
    if (!SetGlobalState<GlobalState>() || !Start<Go_To_Work>())
    {
        throw std::logic_error("Failed to initialize the prototype model.");
    }
}

std::string_view Go_To_Work::Name() const noexcept { return "Go_To_Work"; }
void Go_To_Work::Enter(Context&) noexcept { std::cout << "나무꾼: 자, 오늘도 일하러 가볼까!!" << std::endl; }
void Go_To_Work::Execute(Context& owner) noexcept
{
    if (owner.work_length < 5) {
        std::cout << "나무꾼: 일터로 가는중~(" << owner.work_length << "/5)" << std::endl;
        owner.work_length++;
    }
    else {
        owner.ChangeState<WoodCutting>();
        return;
    }
    
}
void Go_To_Work::Exit(Context&) noexcept { std::cout << "나무꾼: 일터 도착!" << std::endl; }

std::string_view WoodCutting::Name() const noexcept { return "WoodCutting"; }
void WoodCutting::Enter(Context&) noexcept { std::cout << "나무꾼: 도끼질 시작!" << std::endl; }
void WoodCutting::Execute(Context& owner) noexcept
{
    if (owner.stamina > 0) {
        std::cout << "나무꾼: 아직 더할 수 있어 더 나무를 패자고!(" << owner.stamina << "/10)" << std::endl;
        owner.stamina--;
        owner.wood++;
    }
    else {
        owner.ChangeState<Go_To_Sell>();
        return;
    }
}
void WoodCutting::Exit(Context&) noexcept { std::cout << "나무꾼: 이제 지쳤구먼... 마을에 팔러가야겠어" << std::endl; }

std::string_view Go_To_Sell::Name() const noexcept { return "Go_To_Sell"; }
void Go_To_Sell::Enter(Context& owner) noexcept
{
    std::cout << "나무꾼: 이봐! 오늘 작업한 것 팔러왔어!" << std::endl;
    // Interrupt the seller's current activity, including sleep or smoking.
    owner.seller.BeginBuying();
}
void Go_To_Sell::Execute(Context& owner) noexcept
{
    if (owner.wood > 0) {
        if (!owner.seller.CanBuyWood())
        {
            std::cout << "나무꾼: 상점주인이 돌아올 때까지 기다려야겠군." << std::endl;
            return;
        }
        std::cout << "나무꾼: 아직 더 남았어 더 사라고(" << owner.wood << "/10)" << std::endl;
        owner.wood--;
        owner.coin += 5;
    }
    else{
        (void)owner.ChangeState<VisitTavern>();
        return;
    }
}
void Go_To_Sell::Exit(Context& owner) noexcept
{
    std::cout << "나무꾼: 이제 " << owner.coin << "개의 코인이 있어!" << std::endl;
    owner.seller.EndBuying();
}

std::string_view VisitTavern::Name() const noexcept { return "VisitTavern"; }
void VisitTavern::Enter(Context& owner) noexcept
{
    std::cout << "나무꾼: 나무도 다 팔았으니 주점에 들러야겠어!" << std::endl;
    owner.tavernKeeper.BeginVisit(owner);
}
void VisitTavern::Execute(Context& owner) noexcept
{
    std::cout << "나무꾼: 주인장, 술 한 잔 부탁하네!" << std::endl;
    (void)owner.ChangeState<Drinking>();
}
void VisitTavern::Exit(Context&) noexcept {}

std::string_view Drinking::Name() const noexcept { return "Drinking"; }
void Drinking::Enter(Context&) noexcept
{
    std::cout << "나무꾼: 오늘 번 돈으로 목을 축여 볼까." << std::endl;
}
void Drinking::Execute(Context& owner) noexcept
{
    if (owner.coin < 5)
    {
        std::cout << "나무꾼: 술값이 부족하군. 오늘은 돌아가서 자야겠어." << std::endl;
        (void)owner.ChangeState<Sleeping>();
        return;
    }
    if (!owner.tavernKeeper.ServeDrink(owner))
    {
        std::cout << "나무꾼: 지금은 술을 마실 수 없군. 돌아가서 쉬어야겠어." << std::endl;
        (void)owner.ChangeState<Sleeping>();
        return;
    }

    owner.coin -= 5;
    owner.intoxication += 5;
    std::cout << "나무꾼: 나무는 잘 팔았지! 술도 좋구먼. 남은 코인 " << owner.coin
        << ", 취기 " << owner.intoxication << "/20" << std::endl;
    if (owner.intoxication >= 20)
    {
        (void)owner.ChangeState<Sleeping>();
        return;
    }
}
void Drinking::Exit(Context& owner) noexcept
{
    std::cout << "나무꾼: 잘 마셨네. 이제 집에 가서 자야겠어." << std::endl;
    owner.tavernKeeper.EndVisit(owner);
}

std::string_view Sleeping::Name() const noexcept { return "Sleeping"; }
void Sleeping::Enter(Context& owner) noexcept
{
    owner.sleepProgress = 0;
    std::cout << "나무꾼: 집에 도착했으니 푹 자자." << std::endl;
}
void Sleeping::Execute(Context& owner) noexcept
{
    ++owner.sleepProgress;
    std::cout << "나무꾼: 쿨쿨.... " << owner.sleepProgress << "/5" << std::endl;
    if (owner.sleepProgress >= 5)
    {
        // Exit performs recovery before Go_To_Work.Enter is called.
        (void)owner.ChangeState<Go_To_Work>();
        return;
    }
}
void Sleeping::Exit(Context& owner) noexcept
{
    owner.intoxication = 0;
    owner.stamina = 10;
    owner.work_length = 0;
    std::cout << "나무꾼: 술도 깨고 기운도 다 돌아왔어!" << std::endl;
}

std::string_view GlobalState::Name() const noexcept { return "Global"; }
void GlobalState::Enter(Context&) noexcept {}
void GlobalState::Execute(Context&) noexcept
{
    //std::puts("Global.Execute");
    // TODO: Add shared checks here if the scenario needs them.
}
void GlobalState::Exit(Context&) noexcept {}
}
