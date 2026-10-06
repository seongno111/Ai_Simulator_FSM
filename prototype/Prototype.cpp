#include "Prototype.h"
#include <iostream>
#include <cstdio>

namespace prototype
{
Context::Context() : fsm::AIModel<Context>(*this)
{
    // Declare every state used by this model here. AIModel owns the instances.
    RegisterState<Go_To_Work>();
    RegisterState<WoodCutting>();
    RegisterState<Go_To_Sell>();

    RegisterState<GlobalState>();

    // Start last: Enter may access this model and any of its registered states.
    if (!SetGlobalState<GlobalState>() || !Start<Go_To_Work>())
    {
        throw std::logic_error("Failed to initialize the prototype model.");
    }
}

std::string_view Go_To_Work::Name() const noexcept { return "Go_To_Work"; }
void Go_To_Work::Enter(Context&) noexcept { std::cout << "자, 오늘도 일하러 가볼까!!" << std::endl; }
void Go_To_Work::Execute(Context& owner) noexcept
{
    if (owner.work_length < 5) {
        std::cout << "일터로 가는중~(" << owner.work_length << "/5)" << std::endl;
        owner.work_length++;
    }
    else {
        owner.ChangeState<WoodCutting>();
    }
    
}
void Go_To_Work::Exit(Context&) noexcept { std::cout << "일터 도착!" << std::endl; }

std::string_view WoodCutting::Name() const noexcept { return "WoodCutting"; }
void WoodCutting::Enter(Context&) noexcept { std::cout << "도끼질 시작!" << std::endl; }
void WoodCutting::Execute(Context& owner) noexcept
{
    if (owner.stamina > 0) {
        std::cout << "아직 더할 수 있어 더 나무를 패자고!(" << owner.stamina << "/10)" << std::endl;
        owner.stamina--;
        owner.wood++;
    }
    else {
        owner.ChangeState<Go_To_Sell>();
    }
}
void WoodCutting::Exit(Context&) noexcept { std::cout << "이제 지쳤구먼... 마을에 팔러가야겠어" << std::endl; }

std::string_view Go_To_Sell::Name() const noexcept { return "Go_To_Sell"; }
void Go_To_Sell::Enter(Context&) noexcept { std::cout << "이봐! 오늘 작업한 것 팔러왔어!" << std::endl; }
void Go_To_Sell::Execute(Context& owner) noexcept
{
    if (owner.wood > 0) {
        std::cout << "아직 더 남았어 더 사라고" << owner.wood << "/10)" << std::endl;
        owner.wood--;
        owner.coin += 5;
    }
    else{
        owner.work_length = 0;
        owner.stamina = 10;
        owner.ChangeState<Go_To_Work>();
    }
}
void Go_To_Sell::Exit(Context& owner) noexcept { std::cout << "이제 " << owner.coin << "개의 코인이 있어!" << std::endl; }

std::string_view GlobalState::Name() const noexcept { return "Global"; }
void GlobalState::Enter(Context&) noexcept {}
void GlobalState::Execute(Context&) noexcept
{
    std::puts("Global.Execute");
    // TODO: Add shared checks here if the scenario needs them.
}
void GlobalState::Exit(Context&) noexcept {}
}
