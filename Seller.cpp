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

        RegisterState<GlobalState>();

        // Start last: Enter may access this model and any of its registered states.
        if (!SetGlobalState<GlobalState>() || !Start<Open_for_business>())
        {
            throw std::logic_error("Failed to initialize the prototype model.");
        }
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

    std::string_view GlobalState::Name() const noexcept { return "Global"; }
    void GlobalState::Enter(Context&) noexcept {}
    void GlobalState::Execute(Context&) noexcept
    {
        //std::puts("Global.Execute");
        // TODO: Add shared checks here if the scenario needs them.
    }
    void GlobalState::Exit(Context&) noexcept {}
}
