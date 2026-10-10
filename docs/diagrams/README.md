# 시뮬레이터 다이어그램

2026-10-11 현재 코드를 읽어 작성한 구조도다. 시뮬레이터를 빌드하거나 실행해 얻은 로그가 아니다.

| 그림 | PNG | SVG |
| --- | --- | --- |
| FSM 상태도 | [PNG](01_fsm_states.png) | [SVG](01_fsm_states.svg) |
| FSM 메시지·호출 흐름도 | [PNG](02_fsm_message_flow.png) | [SVG](02_fsm_message_flow.svg) |
| Behavior Tree + Blackboard | [PNG](03_behavior_tree.png) | [SVG](03_behavior_tree.svg) |

PNG는 문서·발표 자료 삽입용이고 SVG는 확대하거나 벡터 편집기에 넣어 수정할 때 사용할 수 있다.
한글 SVG 글꼴은 맑은 고딕을 기준으로 한다.

## 읽는 방법

- FSM 상태도: 세 인물을 각각 표시했다. 상점주인의 Global State는 일반 상태에 공통으로 개입하는 별도 영역이다.
  주점 주인의 접대 종료 화살표는 직전 작업 하나로 돌아간다는 의미이며 동시 실행이 아니다.
- 메시지 흐름도: 현재는 메시지 객체·큐·Dispatcher가 없다. 실제 인물 간 동기 함수 호출과 bool 반환을 시퀀스 형태로 나타냈다.
  반복 판매와 반복 음주는 한 묶음으로 표현했으며, 각 세로 위치는 특정 틱 번호를 뜻하지 않는다.
- BT: 위에서 아래로, 들여쓰기로 부모·자식 관계를 표현했다. 형제는 표시 순서대로 평가한다.
  상점주인의 마지막 두 Sequence는 조건과 행동을 한 상자에 묶어 표시했다.
  화장실은 정상 트리를 보존하고, 구매는 평상시 트리를 초기화하며, 주점 접대는 작업 트리를 보존한다.

## 참고 소스

- `../../prototype/WoodCutter.cpp`
- `../../Seller.cpp`
- `../../TavernKeeper.cpp`
- `../../BehaviorTree/WoodCutter.cpp`
- `../../BehaviorTree/Seller.cpp`
- `../../BehaviorTree/TavernKeeper.cpp`
- `../../BehaviorTree/BlackboardKeys.h`
- `../../BehaviorTree/bt/BehaviorTree.h`

재생성 스크립트는 `generate_diagrams.py`다. Python과 Pillow 및 Windows 맑은 고딕 글꼴을 사용한다.
프로젝트 로직이 변경되면 스크립트의 라벨·구조도 갱신해야 한다. 소스를 자동 분석해 그림을 만드는 도구는 아니다.
