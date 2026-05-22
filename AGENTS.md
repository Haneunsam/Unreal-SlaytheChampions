# AGENTS.md - Card System Context & Rules for AI Agents

> **Role & Objective:** You are an expert Unreal Engine 5 C++ developer and systems architect. Your goal is to implement, expand, and maintain the Decoupled Card Action System for the `SlayTheChampions` project.

---

## 1. Project Context & Location
* **Source Root:** `Source/SlayTheChampions/`
* **Engine & Language:** UE 5.6.x / C++ + Blueprint
* **Core Rulebook:** Always strictly follow the coding, commenting, and naming rules specified in `CLAUDE.md`.

---

## 2. Card System Architecture (Summary of CLAUDE.md Section 5)

### 2.1 Pure Decoupling Paradigm
* **No Direct Stat Changes:** `UCard` NEVER modifies Actor stats directly.
* **The Flow:** `UCard` -> `FActionEvent` -> `UActionQueueComponent` -> `EffectResolver`.
* **Single Ownership:** A `UCard*` can only exist in ONE array (`DrawPile`, `HandPile`, `DiscardPile`, `ExhaustPile`) at a time.

### 2.2 Component Boundaries
* `UDeckComponent`: Handles pile state transitions, shuffles (Fisher-Yates), and drawing. No logic or targeting.
* `UCardPlayableComponent`: Validates shared cost, caster status, and target compatibility. No array modifications.
* `UActionQueueComponent`: Manages execution order (Enqueue/Dequeue) and synchronizes loops.

---

## 3. Automated Edge Case Handlers
When writing or refactoring code, ensure these three automated behaviors are preserved:
1. **Over-Draw:** If `HandPile.Num() >= 10`, pop directly to `DiscardPile` instantly.
2. **Auto-Cast:** On turn end, bypass playable checks and force play cards with `Card.Curse.AutoCast` tag.
3. **Ephemeral:** If card has `Card.State.Temporary` tag, wipe references and call `ConditionalBeginDestroy()` immediately upon use/discard.

---

## 4. How to Generate Code
1. Read `CLAUDE.md` first to check the required Korean comment guidelines and English naming rules.
2. Ensure all `TArray<UCard*>` containers have the `UPROPERTY()` macro to prevent GC memory leaks.
3. Write complex rule logic in C++ and expose parameters/VFX hooks to Blueprint.