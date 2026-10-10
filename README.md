# FSM 시나리오 시뮬레이터

## 현재 시나리오: 나무꾼, 상점주인, 주점 주인

현재 실행 코드는 아래 세 인물을 생성하고 100틱 동안 나무꾼 → 상점주인 → 주점 주인 순서로 갱신한다.
빌드·실행은 사용자가 직접 수행한다.

| 파일 | 역할 |
| --- | --- |
| `prototype/WoodCutter.h`, `.cpp` | 나무꾼의 작업·판매·음주·취침 |
| `Seller.h`, `.cpp` | 상점주인의 영업·구매·수면·흡연 |
| `TavernKeeper.h`, `.cpp` | 주점 주인의 설거지·청소·손님 접대 |
| `fsm/AIModel.h` | 모델별 상태 생성과 소유, 타입 기반 상태 전환 |
| `fsm/State.h`, `fsm/StateMachine.h` | 상태 인터페이스와 현재·이전·전역 상태 관리 |

```cpp
Seller::Context seller;
TavernKeeper::Context tavernKeeper;
WoodCutter::Context woodCutter(seller, tavernKeeper);
```

두 가게 주인을 먼저 생성해 나무꾼이 가진 참조보다 오래 살도록 한다.
주점 주인은 방문 중인 나무꾼을 비소유 포인터로 참조한다. 현재는 손님 한 명을 대상으로 한다.

```text
나무꾼: 출근 → 벌목 → 판매 → 주점 방문 → 음주 → 취침 → 출근
상점주인: 나무꾼 판매 Enter에서 Buying, Exit에서 Working
주점 주인: 설거지(5행동) ↔ 청소(10행동)
                     방문 시 접대 → 손님 퇴장 시 이전 작업 복귀
```

- 설거지와 청소 진행도는 `dishProgress`, `cleaningProgress`에 저장한다.
  각각 5번째·10번째 행동에서 즉시 다음 상태로 전환하고, 완료한 작업의 진행도만 0으로 초기화한다.
  접대 때문에 중단할 때는 진행도를 유지한다. 복귀 시 `Enter`에서 초기화하지 않는다.
- 나무꾼의 `VisitTavern::Enter`에서 `BeginVisit`를 호출하면 주점 주인은 `ServingCustomer`로 전환한다.
  방문 상태의 첫 행동에서 나무꾼은 `Drinking`으로 전환한다. 이동 시간은 별도로 두지 않았다.
- 음주 행동마다 주점 주인의 `ServeDrink`를 호출한다. 주점 주인은 손님과 음주 상태, 코인을 확인하고
  술 판매 대사를 출력한다. 나무꾼은 코인 5를 지불하고 취기(`intoxication`)를 5 올리며 대답한다.
  코인과 취기는 나무꾼 쪽에서 한 번만 갱신한다. 주점 주인의 접대 `Execute`는 상태에 맞는 대화를 출력한다.
  따라서 업데이트 순서 때문에 마지막 잔의 판매 대사가 누락되거나 대금이 이중 차감되지 않는다.
- 4번째 잔에서 취기가 20이 되면 즉시 취침으로 전환한다. `Drinking::Exit`에서 `EndVisit`를 호출해
  주점 주인을 `RevertToPreviousState()`로 복귀시킨다. 이후 같은 틱의 주점 업데이트부터 작업을 이어간다.
- 취침은 `sleepProgress`를 5번 증가시킨 뒤 출근으로 전환한다. `Sleeping::Exit`에서
  취기를 0, 스태미나를 10, 출근 거리를 0으로 복구한다. 전환 요청은 `Execute`에서 한다.
  FSM이 `Exit` 안의 중첩 전환을 거부하므로 `Exit`에서는 회복만 수행한다.
- 판매 직후 체력을 회복하던 동작은 제거했다. 코인이 5보다 적거나 술 제공이 거절되면
  음수 잔액이나 무한 대기를 피하도록 취침으로 전환한다.

코드 기준으로 첫 판매 수입은 50코인, 음주 4회 지출은 20코인으로 첫 취침 뒤 잔액은 30코인이다.
아래는 초기 A/B 프로토타입 당시의 구조 설명과 예시를 보존한 것이다.
`prototype/Prototype.*`, `StateA`, `StateB`는 현재 파일·인물 이름이 아니므로 현재 사용은 위 설명을 따른다.

## 초기 FSM 프로토타입 설계 참고

시나리오와 등장인물을 나중에 정의할 수 있도록 만든 C++20 FSM 틀이다.
현재는 특정 이야기, 인물, 능력치, 장소, 자동 전환 조건을 포함하지 않는다.
`main.cpp`의 A → B → A는 상태 머신 API를 설명하기 위한 수동 호출 예시다.

## 참고한 견본

로컬 `C:\AI_Code\Source\VS2010\Buckland_Chapter2-State Machines`의
`WestWorldWithWoman/State.h`, `StateMachine.h`, `Miner.h`, `Miner.cpp`,
`MinerOwnedStates.cpp`, `main.cpp`를 참고했다.
Mat Buckland 견본의 상태 패턴을 바탕으로 새로 작성했으며, 광부와 가족의 행동은 가져오지 않았다.

핵심 구조는 동일하다. 소유 객체가 데이터를 보관하고, 상태가 행동을 구현하며,
상태 머신이 현재·이전·전역 상태와 전환 순서를 관리한다.
이 FSM의 AI는 작성자가 정의한 규칙에 따라 행동을 선택하는 구조이며 학습 모델은 포함하지 않는다.

## 파일 구성

| 파일 | 역할 |
| --- | --- |
| `fsm/State.h` | `State<Owner>` 인터페이스: 이름, 진입, 실행, 종료 |
| `fsm/StateMachine.h` | 재사용 가능한 템플릿 FSM 전체 구현 |
| `fsm/AIModel.h` | 모델 공통 기반 클래스: 상태 생성·소유, 타입 기반 전환, 가상 Update |
| `prototype/Prototype.h` | 임시 소유 객체 `Context`와 상태 선언 |
| `prototype/Prototype.cpp` | A/B 상태와 선택적 전역 상태의 로그 출력 |
| `main.cpp` | AI 모델 하나 생성 후 업데이트, 전환, 복귀 연결 예시 |

템플릿은 사용 지점에 정의가 보여야 하므로 `StateMachine` 구현도 헤더에 있다.
Visual Studio 프로젝트와 필터에 파일을 등록했다. 기존 C++20, v145 및 플랫폼 설정은 유지했다.

## 역할과 호출 순서

`Owner`는 나중에 작성할 등장인물이나 시뮬레이션 컨텍스트 타입이다.
`State<Owner>`를 상속하면 상태 함수에서 `Owner&`를 받아 데이터를 읽거나 바꿀 수 있다.
현재 `prototype::Context`는 `AIModel<Context>`를 상속하는 중립적인 모델이다.
모델 생성자에서 모든 상태 객체를 등록하고 최초 상태를 시작한다.
클래스 자체의 선언은 헤더에 두고, 해당 클래스의 **객체 생성**을 모델 생성자에 모은 구조다.

```cpp
Context::Context() : fsm::AIModel<Context>(*this)
{
    RegisterState<StateA>();
    RegisterState<StateB>();
    RegisterState<GlobalState>();

    if (!SetGlobalState<GlobalState>() || !Start<StateA>())
        throw std::logic_error("Failed to initialize the prototype model.");
}
```

호출하는 쪽은 `prototype::Context model;` 하나만 선언하면 된다.
`model.Update()`는 상속받은 가상 함수를 실행하며 내부에서 FSM을 갱신한다.
상태 객체를 외부 변수로 만들 필요 없이 `model.ChangeState<prototype::StateB>()`로 전환한다.

상속은 두 계층으로 나뉜다.

- **AI 모델**은 `AIModel<자기타입>`을 상속한다. 공통 `Update()`를 그대로 사용하거나 재정의한다.
- **상태**는 `State<모델타입>`을 상속한다. `Name`, `Enter`, `Execute`, `Exit`를 각각 재정의한다.

`Update()`는 모델 전체의 한 틱, `Execute()`는 개별 상태의 행동을 담당한다.
모든 등록 상태의 `Execute()`를 한꺼번에 호출하지 않으며, 전역 상태와 현재 상태만 실행한다.
함수 이름은 `Execute()`이다.

인물별 갱신이 필요하면 모델 클래스에 다음처럼 선언하고 구현할 수 있다.

```cpp
// Context 클래스 내부에 추가하는 예시
[[nodiscard]] bool Update() noexcept override
{
    // TODO: 이 인물의 공통 데이터 갱신
    return fsm::AIModel<Context>::Update();
}
```

기반 클래스의 `Update()`를 호출해야 FSM도 실행된다.
상태 내부에서는 전달받은 `owner`로 모델 데이터와 상속받은 기능에 접근한다.

```cpp
// 상태 전환을 작성하는 방법. 현재 프로토타입에는 자동 전환 조건이 없다.
// Execute(Context& owner) 내부에서 조건을 확인한 후:
(void)owner.ChangeState<StateB>();
return;
```

```text
Context : AIModel<Context> (나중에 인물 데이터 추가)
  ├─ 등록 상태 객체 소유: StateA, StateB, GlobalState
  └─ StateMachine<Context>
       ├─ current  → 현재 상태
       ├─ previous → 직전 상태
       └─ global   → 선택적 공통 실행 훅

Start(A)       : A.Enter
Update()       : Global.Execute → Current.Execute
ChangeState(B) : A.Exit → previous=A → current=B → B.Enter
Revert()       : B.Exit → previous=B → current=A → A.Enter
```

- `Enter`: 해당 상태로 진입할 때 한 번 호출한다.
- `Execute`: `Update()` 한 번마다 호출한다. 한 번의 업데이트가 한 틱이다.
- `Exit`: 다른 상태로 전환할 때 한 번 호출한다.
- 전역 상태는 매 틱 실행할 공통 점검용이다. 이 프로토타입에서는 `Execute`만 호출하며 `Enter`와 `Exit`는 호출하지 않는다.

## 모델에서 사용하는 API

| 함수 | 동작 |
| --- | --- |
| `RegisterState<T>(생성자 인자...)` | protected. 모델 생성자에서 상태 생성·등록. 타입별 한 개만 허용 |
| `Start<T>()` | protected. 등록한 최초 상태의 Enter 호출 |
| `SetGlobalState<T>()` | protected. 시작 전에 등록한 전역 상태 지정 |
| `Update()` | public virtual. 전역 상태와 현재 상태 실행. 모델에서 override 가능 |
| `ChangeState<T>()` | public. 이 모델에 등록된 T 객체로 전환 |
| `RevertToPreviousState()` | public. 직전 상태로 복귀 |
| `IsInState<T>()` | public. 현재 상태가 이 모델에 등록된 T인지 확인 |
| `GetFSM()` | public. 현재·이전·전역 상태 조회를 위한 const 참조 |

등록하지 않은 타입으로 시작·전환·전역 설정·상태 확인을 요청하면 `false`를 반환한다.
다른 모델용 상태 타입을 전달하면 `static_assert`로 컴파일 시 검출한다.
중복 등록 또는 시작 이후 등록은 `std::logic_error`를 발생시킨다.
상태 생성은 메모리 할당을 포함하므로 모델 생성자는 `noexcept`가 아니다.
조회는 등록된 상태 목록을 순회하며 정확한 타입을 비교한다. 기본 RTTI 설정을 사용한다.

## 내부 StateMachine API

다음은 기반 클래스가 사용하는 저수준 API다. 일반 모델 코드는 위의 타입 기반 API를 사용한다.

| 함수 | 동작 및 반환값 |
| --- | --- |
| `Start(state)` | 최초 상태를 설정하고 `Enter` 호출. 이미 시작했다면 `false` |
| `SetGlobalState(pointer)` | 시작 전 전역 훅 설정. `nullptr`로 해제 가능. 시작 후에는 `false` |
| `Update()` | 전역 상태와 현재 상태 실행. 시작 전이나 재진입 시 `false` |
| `ChangeState(state)` | 즉시 전환. 시작 전이면 최초 진입. 동일 객체로 전환하거나 진입/종료 콜백 안에서 요청하면 `false` |
| `RevertToPreviousState()` | 직전 상태로 전환. 이전 상태가 없으면 `false` |
| `IsInState(state)` | 현재 상태가 전달한 **동일 객체**인지 확인 |
| `CurrentState()`, `PreviousState()`, `GlobalState()` | 읽기 전용 상태 포인터 조회. 설정되지 않았다면 `nullptr` |

`true`는 작업 수행, `false`는 거부 또는 변화 없음을 뜻한다.
조회한 포인터는 null 여부를 확인한 뒤 `Name()` 등을 호출한다.
반환값에 `[[nodiscard]]`를 붙였으므로 호출 결과를 확인하거나 의도적으로 무시할 때 `(void)`로 명시한다.

## 전환 규칙과 객체 수명

1. `AIModel`이 등록 상태를 `unique_ptr`로 소유하고 자동으로 해제한다.
   내부 `StateMachine`은 그 객체의 비소유 포인터만 보관한다.
   상태 목록보다 FSM이 먼저 파괴되도록 멤버 선언 순서를 지정했다.
   모델을 여러 개 생성하면 각 모델에 별도의 상태 인스턴스가 생긴다.
2. 소유 객체의 주소가 바뀌면 참조가 무효화되므로 FSM의 복사·이동을 금지했다.
   여러 인물을 컨테이너에 보관할 때는 `std::vector<std::unique_ptr<Character>>`처럼 인물 주소를 유지한다.
3. `Execute` 안에서 전환할 수 있다. 전환은 즉시 적용되지만 원래 `Execute` 함수가 자동으로 끝나지는 않는다.
   전환 호출 뒤 `return;`을 써서 이전 상태의 행동이 계속되지 않도록 한다.
4. `Enter`와 `Exit` 안의 중첩 전환은 `false`로 거부한다.
   콜백에서 같은 FSM의 `Update`를 재호출해도 `false`다.
5. 전역 `Execute`에서 전환하면 새 현재 상태의 `Execute`가 같은 틱에 실행된다.
   현재 상태의 `Execute`에서 전환하면 새 상태의 `Enter`까지만 즉시 호출되고, 새 상태의 `Execute`는 다음 틱에 실행된다.
6. 이전 상태는 한 개만 기억한다. 복귀를 반복하면 두 상태 사이를 왕복하며, 이력 스택처럼 더 과거로 돌아가지 않는다.
7. 콜백은 `noexcept` 계약이다. 구현에서 예외가 밖으로 나가면 프로그램이 종료된다.
   실패할 수 있는 작업은 콜백 안에서 처리하거나 소유 객체의 오류 데이터로 전달한다.
8. 단일 스레드용이다. 소멸자는 `Exit`를 호출하지 않는다. 파일 등 자원은 RAII로 관리하고,
   시나리오 종료 절차가 필요하면 나중에 종료 상태를 명시적으로 설계한다.

견본과 달리 최초 진입에서도 `Enter`를 호출하고, 빈 상태·중복 전환·재진입을 검사한다.
싱글턴 상태를 강제하지 않으며, `IsInState`는 타입이 아닌 객체 주소를 비교한다.
모델 계층은 타입당 상태 객체 하나만 등록하므로 `IsInState<T>()`로 편리하게 조회할 수 있다.

## 나중에 시나리오를 추가하는 순서

1. 등장인물과 필요한 데이터, 세계 정보를 정한다.
2. 인물 클래스를 `fsm::AIModel<인물타입>`에서 상속하고 필요한 데이터를 추가한다.
   생성자 초기화 목록에서 `fsm::AIModel<인물타입>(*this)`로 연결한다.
3. 각 상태를 `fsm::State<인물타입>`에서 상속하고 네 가상 함수를 구현한다.
4. 인물 생성자에서 모든 상태를 `RegisterState<상태타입>()`으로 등록한다.
   필요하면 `SetGlobalState<전역상태>()`를 호출하고, 인물 데이터 초기화가 끝난 마지막에
   `Start<최초상태>()`를 호출한다. 더 파생할 클래스가 있다면 가장 최종 클래스에서 시작한다.
5. `Execute`에서 조건을 확인하고 `owner.ChangeState<대상상태>()`를 호출한 뒤 반환한다.
6. 시뮬레이터 루프에서 인물마다 `Update()`를 한 번씩 호출한다.
   시간 간격, 인물 갱신 순서, 종료 조건은 시나리오가 정해진 뒤 추가한다.

현재 메시지 전달, 지연 이벤트, 인물 관리자, 파일 저장, 화면 UI는 포함하지 않는다.
필요한 기능은 시나리오 확정 후 별도 계층으로 추가할 수 있다.

## 프로토타입 실행 시 예상 출력

아래는 코드를 기준으로 작성한 예상 결과이며 실제 실행 결과가 아니다.

```text
FSM prototype: A -> B -> previous (A)
StateA.Enter
Global.Execute
StateA.Execute
StateA.Exit
StateB.Enter
Global.Execute
StateB.Execute
StateB.Exit
StateA.Enter
Global.Execute
StateA.Execute
```

사용자가 `FSM.slnx`를 열고 직접 빌드·실행한다.
작성 과정에서는 빌드나 실행을 하지 않았다.
