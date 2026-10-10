#include "TavernKeeper.h"
#include "prototype/WoodCutter.h"

#include <iostream>

namespace TavernKeeper
{
Context::Context() : fsm::AIModel<Context>(*this)
{
    RegisterState<WashingDishes>();
    RegisterState<Cleaning>();
    RegisterState<ServingCustomer>();
    if (!Start<WashingDishes>())
        throw std::logic_error("Failed to initialize the tavern keeper.");
}

void Context::BeginVisit(WoodCutter::Context& visitor) noexcept
{
    // Repeated notifications must not overwrite the saved previous state.
    if (customer != nullptr)
        return;

    customer = &visitor;
    if (!ChangeState<ServingCustomer>())
        customer = nullptr;
}

void Context::EndVisit(WoodCutter::Context& visitor) noexcept
{
    if (customer != &visitor)
        return;

    customer = nullptr;
    if (IsInState<ServingCustomer>())
        (void)RevertToPreviousState();
}

bool Context::ServeDrink(WoodCutter::Context& visitor) noexcept
{
    if (customer != &visitor || !IsInState<ServingCustomer>()
        || !visitor.IsInState<WoodCutter::Drinking>() || visitor.coin < 5)
        return false;

    // Called once per drinking action, including the last drink before departure.
    std::cout << "주점 주인: 술 한 잔에 5코인이오. 오늘도 고생 많았네!" << std::endl;
    return true;
}

std::string_view WashingDishes::Name() const noexcept { return "WashingDishes"; }
void WashingDishes::Enter(Context& owner) noexcept
{
    std::cout << "주점 주인: 설거지를 해야겠어." << std::endl;
}
void WashingDishes::Execute(Context& owner) noexcept
{
    ++owner.dishProgress;
    std::cout << "주점 주인: 접시를 닦는 중. " << owner.dishProgress << "/5" << std::endl;
    if (owner.dishProgress >= 5)
    {
        owner.dishProgress = 0;
        (void)owner.ChangeState<Cleaning>();
        return;
    }
}
void WashingDishes::Exit(Context&) noexcept {}

std::string_view Cleaning::Name() const noexcept { return "Cleaning"; }
void Cleaning::Enter(Context& owner) noexcept
{
    std::cout << "주점 주인: 이제 청소를 해야겠군. " << std::endl;
}
void Cleaning::Execute(Context& owner) noexcept
{
    ++owner.cleaningProgress;
    std::cout << "주점 주인: 바닥을 쓸고 닦는 중. " << owner.cleaningProgress << "/10" << std::endl;
    if (owner.cleaningProgress >= 10)
    {
        owner.cleaningProgress = 0;
        (void)owner.ChangeState<WashingDishes>();
        return;
    }
}
void Cleaning::Exit(Context&) noexcept {}

std::string_view ServingCustomer::Name() const noexcept { return "ServingCustomer"; }
void ServingCustomer::Enter(Context&) noexcept
{
    std::cout << "주점 주인: 어서 오게! 하던 일은 잠시 멈추지." << std::endl;
}
void ServingCustomer::Execute(Context& owner) noexcept
{
    if (owner.customer == nullptr)
    {
        (void)owner.RevertToPreviousState();
        return;
    }
    if (owner.customer->IsInState<WoodCutter::VisitTavern>())
        std::cout << "주점 주인: 여기 앉게. 술을 준비해 두겠네." << std::endl;
    else if (owner.customer->IsInState<WoodCutter::Drinking>())
        std::cout << "주점 주인: 오늘 나무는 잘 팔았는가?" << std::endl;
    else
        owner.EndVisit(*owner.customer);
}
void ServingCustomer::Exit(Context&) noexcept
{
    std::cout << "주점 주인: 잘 가게! 하던 일을 마저 해야겠군." << std::endl;
}
}
