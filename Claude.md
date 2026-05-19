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

## Project
- Engine: UE 5.6.x
- Language: C++ + Blueprint
- Project: SlayTheChampions
- Source: SlayTheChampions/Source/SlayTheChampions/
- 모듈 구조:
  - `Public/CombatKernel/` — 전투 로직 헤더
  - `Public/GameManagers/` — 게임 매니저 헤더
  - `Public/Map/` — 맵 헤더
  - `Private/CombatKernel/` — 전투 로직 구현
  - `Private/GameManagers/` — 게임 매니저 구현
  - `Private/Map/` — 맵 구현

## Code Navigation
- 심볼/클래스 위치 찾을 때: `grep "이름" tags` 먼저 확인
- 파일 추가/삭제 후 태그 갱신:
  `ctags -R --c++-kinds=+pf --fields=+iaSn --extras=+q --languages=C++ --exclude=Intermediate --exclude=Saved --exclude=Binaries --exclude=.git SlayTheChampions/Source/`
- UHT 규칙: 프로젝트 전체에서 헤더 파일명 중복 불가 (서브폴더 포함)

## Coding Rules
- 복잡한 로직 → C++
- 입력 처리, VFX 연결 → Blueprint
- UPROPERTY / UFUNCTION 매크로 필수
- 헤더엔 전방선언 우선, 꼭 필요할 때만 #include
- 새 파일은 반드시 해당 모듈 서브폴더에 생성 (루트 Public/Private 직접 추가 금지)