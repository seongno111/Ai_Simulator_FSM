#include "Seller.h"
#include <iostream>
#include <cstdio>

namespace Seller
{
    Context::Context() : fsm::AIModel<Context>(*this)
    {
        // Declare every state used by this model here. AIModel owns the instances.
        RegisterState<Open_for_business>();
        RegisterState<Working>();
        RegisterState<Buying>();
        RegisterState<Sleep>();
        RegisterState<Cigarette>();
        RegisterState<Toilet>();

        RegisterState<GlobalState>();

        // Start last: Enter may access this model and any of its registered states.
        if (!SetGlobalState<GlobalState>() || !Start<Open_for_business>())
        {
            throw std::logic_error("Failed to initialize the prototype model.");
        }
    }

    void Context::BeginBuying() noexcept
    {
        buyingRequested = true;
        if (IsInState<Toilet>())
        {
            tradeChangedDuringToilet = true;
            return;
        }
        (void)ChangeState<Buying>();
    }

    void Context::EndBuying() noexcept
    {
        buyingRequested = false;
        if (IsInState<Toilet>())
        {
            tradeChangedDuringToilet = true;
            return;
        }
        (void)ChangeState<Working>();
    }

    bool Context::CanBuyWood() const noexcept
    {
        return IsInState<Buying>();
    }

    std::string_view Open_for_business::Name() const noexcept { return "Open_for_business"; }
    void Open_for_business::Enter(Context&) noexcept { std::cout << "상점주인: 어후, 오늘도 날이 밝았구먼. 가게문이나 열어 둬야지..." << std::endl; }
    void Open_for_business::Execute(Context& owner) noexcept
    {
        owner.ChangeState<Working>();
    }
    void Open_for_business::Exit(Context&) noexcept { std::cout << "상점주인: 그래, 오늘도 일해야지." << std::endl; }

    std::string_view Working::Name() const noexcept { return "Working"; }
    void Working::Enter(Context&) noexcept { std::cout << "상점주인: 영업 시작." << std::endl; }
    void Working::Execute(Context& owner) noexcept
    {
        if (owner.sleep > 10) {
            owner.ChangeState<Sleep>();
            return;
        }

        if (owner.cigarette > 5) {
            owner.ChangeState<Cigarette>();
            return;
        }
        std::cout << "상점주인: 가게 보는 중...." << owner.sleep << ", " << owner.cigarette << std::endl;
        owner.sleep++;
        owner.cigarette++;
    }
    void Working::Exit(Context&) noexcept {  }

    std::string_view Buying::Name() const noexcept { return "Buying"; }
    void Buying::Enter(Context&) noexcept
    {
        std::cout << "상점주인: 가져온 나무를 한번 볼까?" << std::endl;
    }
    void Buying::Execute(Context&) noexcept
    {
        // Stay here until the woodcutter leaves Go_To_Sell.
        std::cout << "상점주인: 나무를 사는 중...." << std::endl;
    }
    void Buying::Exit(Context&) noexcept
    {
        std::cout << "상점주인: 거래 끝! 다시 가게를 봐야지." << std::endl;
    }

    std::string_view Sleep::Name() const noexcept { return "Sleep"; }
    void Sleep::Enter(Context&) noexcept { std::cout << "상점주인: 하~암, 어후 졸려 한숨 자야겠군...." << std::endl; }
    void Sleep::Execute(Context& owner) noexcept
    {
        if (owner.sleep > 0) {
            std::cout << "상점주인: ZZZZZZ" << std::endl;
            owner.sleep--;
        }
        else {
            owner.ChangeState<Working>();
            return;
        }
    }
    void Sleep::Exit(Context& owner) noexcept { std::cout << "상점주인: 어으 잘잤다." << std::endl; }

    std::string_view Cigarette::Name() const noexcept { return "Cigarette"; }
    void Cigarette::Enter(Context& owner) noexcept { 
        std::cout << "상점주인: 담배나 한대 태우고 와야겠어." << std::endl;
        if (!owner.resumingFromToilet)
            owner.cigarette = 5;
    }
    void Cigarette::Execute(Context& owner) noexcept
    {
        if (owner.cigarette > 0) {
            std::cout << "상점주인: 스읍, 후우~" << owner.cigarette << "/5" << std::endl;
            owner.cigarette--;
        }
        else {
            owner.ChangeState<Working>();
            return;
        }
    }
    void Cigarette::Exit(Context& owner) noexcept { std::cout << "상점주인: 다시 돌아가야지..." << std::endl; }

    std::string_view Toilet::Name() const noexcept { return "Toilet"; }
    void Toilet::Enter(Context& owner) noexcept
    {
        owner.toiletProgress = 0;
        owner.tradeChangedDuringToilet = false;
        std::cout << "상점주인: 급하군! 하던 일은 멈추고 화장실부터 가야겠어." << std::endl;
    }
    void Toilet::Execute(Context& owner) noexcept
    {
        ++owner.toiletProgress;
        std::cout << "상점주인: 화장실 이용 중.... " << owner.toiletProgress << "/5" << std::endl;
        if (owner.toiletProgress < 5)
            return;

        if (owner.tradeChangedDuringToilet)
        {
            if (owner.buyingRequested)
                (void)owner.ChangeState<Buying>();
            else
                (void)owner.ChangeState<Working>();
            return;
        }
        // Re-entering Cigarette must not reset its remaining progress.
        owner.resumingFromToilet = true;
        (void)owner.RevertToPreviousState();
        owner.resumingFromToilet = false;
    }
    void Toilet::Exit(Context& owner) noexcept
    {
        owner.toilet = 0;
        std::cout << "상점주인: 이제 괜찮군. 하던 일을 이어가자." << std::endl;
    }

    std::string_view GlobalState::Name() const noexcept { return "Global"; }
    void GlobalState::Enter(Context&) noexcept {}
    void GlobalState::Execute(Context& owner) noexcept
    {
        if (owner.IsInState<Toilet>())
            return;
        ++owner.toilet;
        if (owner.toilet >= 10)
            (void)owner.ChangeState<Toilet>();
    }
    void GlobalState::Exit(Context&) noexcept {}
}
