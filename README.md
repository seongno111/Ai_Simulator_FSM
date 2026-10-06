# FSM 시나리오 시뮬레이터 프로토타입

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
| `prototype/Prototype.h` | 임시 소유 객체 `Context`와 상태 선언 |
| `prototype/Prototype.cpp` | A/B 상태와 선택적 전역 상태의 로그 출력 |
| `main.cpp` | 객체 생성, 시작, 업데이트, 전환, 복귀 연결 예시 |

템플릿은 사용 지점에 정의가 보여야 하므로 `StateMachine` 구현도 헤더에 있다.
Visual Studio 프로젝트와 필터에 파일을 등록했다. 기존 C++20, v145 및 플랫폼 설정은 유지했다.

## 역할과 호출 순서

`Owner`는 나중에 작성할 등장인물이나 시뮬레이션 컨텍스트 타입이다.
`State<Owner>`를 상속하면 상태 함수에서 `Owner&`를 받아 데이터를 읽거나 바꿀 수 있다.
현재 `prototype::Context`는 FSM 멤버만 가진 연결용 자리다.

```text
Context (나중에 인물 데이터 추가)
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

## API

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

1. FSM은 소유 객체나 상태 객체를 생성·삭제하지 않는다. 참조와 비소유 포인터만 보관한다.
   `main.cpp`처럼 상태를 먼저 생성하고 소유 객체를 나중에 생성하면 역순 파괴 시 수명이 맞는다.
   나중에 인물 클래스가 상태를 멤버로 가지면 상태 멤버를 FSM 멤버보다 먼저 선언한다.
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
같은 상태 객체를 여러 인물이 공유하려면 인물별 가변 데이터는 상태가 아닌 인물 객체에 둔다.

## 나중에 시나리오를 추가하는 순서

1. 등장인물과 필요한 데이터, 세계 정보를 정한다.
2. 인물 타입에 데이터와 `fsm::StateMachine<인물타입>` 멤버를 추가한다.
   생성자에서 `machine(*this)`로 연결하고, 초기 진입은 인물 및 상태 객체 구성이 끝난 뒤 호출한다.
3. 각 상태를 `fsm::State<인물타입>`에서 상속하고 네 가상 함수를 구현한다.
4. 상태 전환 대상은 인물의 상태 멤버나 상태 조회 함수를 통해 접근하도록 연결한다.
5. `Execute`에서 조건을 확인하고 `owner.machine.ChangeState(대상상태)`를 호출한 뒤 반환한다.
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
