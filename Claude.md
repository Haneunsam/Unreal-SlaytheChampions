# CLAUDE.md

Behavioral guidelines to reduce common LLM coding mistakes. Merge with project-specific instructions as needed.

**Tradeoff:** These guidelines bias toward caution over speed. For trivial tasks, use judgment.

## 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:
- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

## 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:
- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

## 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:
- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

---

**These guidelines are working if:** fewer unnecessary changes in diffs, fewer rewrites due to overcomplication, and clarifying questions come before implementation rather than after mistakes.

## 주석
VisualStudio를 이용하여 코드를 짤때 무슨 역할을 하는지 항상 주석처리를 한다.

## Project
- Engine: UE 5.6.x
- Language: C++ + Blueprint
- Source: Source/SlayTheChampions/

## Code Navigation
- 심볼/클래스 위치 찾을 때: `grep "이름" tags` 먼저 확인
- 파일 추가/삭제 후 태그 갱신:
  `ctags -R --c++-kinds=+pf --fields=+iaSn --extras=+q --languages=C++ --exclude=Intermediate --exclude=Saved --exclude=Binaries --exclude=.git Source/`

## Coding Rules
- 복잡한 로직 → C++
- 입력 처리, VFX 연결 → Blueprint
- UPROPERTY / UFUNCTION 매크로 필수
- 헤더엔 전방선언 우선, 꼭 필요할 때만 #include
- UE_LOG 메시지는 영어로 작성 (팀원 PC 인코딩 오류 방지)

## [Role & Instructions]
You are an expert developer. When modifying or refactoring code, you must strictly adhere to the following commenting and naming rules. These rules apply to both modifying existing code and writing new code.

## [Strict Rules]
1. Do NOT delete comments: Never remove, omit, or delete any existing comments during the refactoring or modification process. Keep them intact unless the underlying logic changes fundamentally.
2. Comments in Korean: All explanations, inline comments, and docstrings must be written EXCLUSIVELY in Korean (한국어).
3. Variables in English: All variable names, function names, class names, and identifiers must be written EXCLUSIVELY in English. Do not use Korean for any identifiers.

---

## 카드 시스템

### 파일 목록 및 역할

| 파일 | 역할 |
|------|------|
| CardDataTypes.h | Enum 4종 + FCardDataRow Struct |
| CardSubsystem.h/cpp | DT_Cards 로드/조회 (GameInstance Subsystem) |
| StarterDeckRow.h | DT_StarterDeck용 Row Struct (CardID 하나만 보유) |
| HandComponent.h/cpp | 덱/손패/버리기 더미 관리 (ActorComponent) |
| PartyMemberPawn.h/cpp | HandComponent 소유 Pawn (BP_Pawn1, BP_Pawn2 부모) |
| CardSaveGame.h/cpp | 덱 데이터 파일 저장/불러오기 (USaveGame) |
| CardStyleDataAsset.h/cpp | 직업별+희귀도별 카드 스타일 이미지 (UDataAsset) |
| CardWidget.h/cpp | 카드 한 장 위젯 C++ 베이스 (WBP_Card 부모) |

### 사용하지 않는 파일
- CardLog.h/cpp (미사용 - 코드에서 제외)
- PlayerDeckData.h (CardSaveGame으로 대체 - 삭제 가능)

---

### 데이터 흐름
```
DT_Cards (카드 스펙 DB)
    └─ CardSubsystem 에서 카드 ID(FName)로 조회
    └─ FCardDataRow 반환

DT_StarterDeck_Warrior/Mage (초기 덱 구성)
    └─ CardSaveGame::LoadOrCreate() 로 최초 SaveGame 생성

CardSaveGame (USaveGame - 슬롯명: "PlayerDeckSave")
    └─ PartyMemberPawn::InitializeDeck() 에서 덱 불러옴
    └─ PartyMemberPawn::SaveDeckToSaveGame() 에서 전투 종료 시 저장

DA_CardStyle_Warrior/Mage/Healer (UCardStyleDataAsset)
    └─ CardWidget::SetCardData() 에서 직업별+희귀도별 이미지 적용
```

---

### 덱 저장 방식 (A방식 - 슬더스 방식)
```
전투 종료 시: DrawPile + Hand + DiscardPile 합쳐서 SaveGame 저장
다음 전투 시작 시: SaveGame 에서 불러와서 셔플 후 사용
카드 보상 시: SaveGame 에 즉시 추가 저장
게임 최초 시작 시: DT_StarterDeck 에서 읽어서 SaveGame 생성
```

### SaveGame 사용법
```cpp
// 게임 시작 시 1회 호출 (LevelManager::Initialize 권장)
UCardSaveGame::LoadOrCreate(DT_StarterDeck_Warrior, DT_StarterDeck_Mage);

// 전투 종료 시
Pawn->SaveDeckToSaveGame();

// 카드 보상 시
UCardSaveGame::AddCard(PawnIndex, CardName);

// 카드 제거 시
UCardSaveGame::RemoveCard(PawnIndex, CardName);
```

---

### PartyMemberPawn 설정
에디터에서 BP_Pawn1, BP_Pawn2 생성 시 반드시 설정:
```
PawnIndex        : 0 (Pawn1), 1 (Pawn2)
JobClass         : Warrior / Mage / Healer
OverrideDeckNames: 테스트용 직접 지정 (비워두면 SaveGame 사용)
```

덱 초기화 우선순위:
```
1순위: SaveGame 에서 불러옴
2순위: OverrideDeckNames 직접 지정 (테스트용)
3순위: CardSubsystem 에서 직업 기반 자동 조회
```

---

### WBP_Card 위젯 구성
BindWidget 변수명과 에디터 위젯 이름이 반드시 일치해야 함:
```
BorderOuter      Image   직업별 카드 외부 테두리 배경
BorderInner      Image   직업별 카드 내부 배경
RarityBorder     Image   희귀도별 카드 그림 테두리
MainImage        Image   카드마다 다른 그림 (PaperSprite)
TypeBackground   Image   희귀도별 카드 속성 텍스트 배경
GemImage         Image   직업별 좌상단 보석
CostText         Text    코스트 숫자
CardNameText     Text    카드 이름
DescriptionText  Text    카드 설명 (태그 자동 치환)
```

### Description 태그 시스템
DT_Cards Description 에 태그 사용 시 실제 수치로 자동 치환:
```
{Damage}      피해량
{Block}       방어도
{DrawCount}   드로우 수
{UsingCount}  사용 횟수
{HealAmount}  회복량
{EffectValue} 특수 효과 수치
```
예시: `"적에게 {Damage} 피해를 입힙니다."` → `"적에게 6 피해를 입힙니다."`

---

### DA_CardStyle 구성 (CardStyleDataAsset)
```
직업별 (희귀도 무관)
    BorderOuter          카드 외부 테두리 배경 (PaperSprite)
    BorderInner          카드 내부 배경 (PaperSprite)
    GemImage             좌상단 보석 1종 (PaperSprite)

희귀도별 (Normal / Rare / Legendary)
    RarityBorder_*       카드 그림 테두리 (PaperSprite)
    TypeBackground_*     카드 속성 텍스트 배경 (PaperSprite)
```
생성 방법: Content Browser → Miscellaneous → Data Asset → `CardStyleDataAsset` 검색

---

### DataTable 목록
| 파일 | Row Struct | 역할 |
|------|------------|------|
| DT_Cards | FCardDataRow | 전체 카드 스펙 DB |
| DT_StarterDeck_Warrior | FStarterDeckRow | Warrior 시작 덱 (공격6 수비5) |
| DT_StarterDeck_Mage | FStarterDeckRow | Mage 시작 덱 (미완성) |

---

### Build.cs 모듈
```
UMG       UserWidget, UImage, UTextBlock
Slate
SlateCore
Paper2D   UPaperSprite (카드/스타일 이미지)
```

---

### 미완성 작업
- DT_StarterDeck_Mage, Healer 생성
- DA_CardStyle 전체 이미지 지정
- WBP_Hand (손패 전체 위젯) 제작
- OnHandChanged 델리게이트 연결
- LevelManager::Initialize 에서 LoadOrCreate 호출 연결

[Task]
(Here, describe the specific task you want to achieve.)
