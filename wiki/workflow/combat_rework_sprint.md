# Sprint: Combat Rework - Sprint 1

**Status**: Planning
**Goal**: Prototype the "Stamina as Shield" mechanic and rebalance Heavy Attacks.

## Backlog

### Analysis (In Progress)
- [ ] Analyze `mwmechanics/combat.cpp` to map current damage flow.
- [ ] Analyze `mwmechanics/creaturestats.hpp` to understand Stamina (Fatigue) modification.

### Core Mechanics (ToDo)
- [ ] **Implement Stamina Damage Buffer**:
    -   Modify damage application logic: check Stamina first.
    -   Subtract damage from Stamina.
    -   Apply overflow to Health.
- [ ] **Rework Heavy Attacks**:
    -   Increase charge time requirement.
    -   Apply damage multiplier based on charge duration.
    -   Increase Stamina cost for heavy attacks.
    -   **Refactor**: Ensure these values are easily tweakable (constants or settings).

### Balancing (ToDo)
- [ ] **Stamina Economy Pass 1**:
    -   Increase base Stamina Regen rate.
    -   Increase Attack Stamina Cost.
- [ ] **Hit Chance Tweak**:
    -   Temporarily force high hit chance for testing stamina mechanics.

## Issues / Blockers
- None currently.

## Notes & Decisions
- *2025-12-17*: Initial sprint plan created.
