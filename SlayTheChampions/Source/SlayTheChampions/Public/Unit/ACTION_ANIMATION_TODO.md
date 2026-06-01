# Action Animation Component — TODO

## 목표
플레이어/적 유닛에 액션 애니메이션 컴포넌트를 붙여서
카드 실행 / 적 행동 시 애니메이션을 트리거할 수 있게 만들기.

## 구현 예정 내용
- `UActionAnimComponent` (또는 유사 이름) 컴포넌트 생성
- AUnit (플레이어 / 적 공통) 에 추가
- CombatManager::OnActionExecuted, OnEnemyTurnStart 델리게이트를 구독
- 카드 종류 / 적 행동 종류에 따라 알맞은 애니메이션 재생

## 연결 포인트
- CombatManager::OnActionExecuted(FCardDataRow Card, int32 CasterIndex)
- CombatManager::OnEnemyTurnStart(int32 EnemyIndex)
