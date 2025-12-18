---
description: Sprint 8 - Active Blocking System
---

# Sprint 8: Active Blocking Workflow

This sprint focuses on implementing a manual, skill-based blocking system to replace or augment the passive RNG blocking.

## 1. Input Handling
- **Task**: Map a key (defaulting to the same toggle as "Ready Weapon" or a new dedicated key) to trigger the "Block" state.
- **File**: `apps/openmw/mwinput/inputmanagerimp.cpp` and `mwmechanics/movement.hpp`.

## 2. Animation Logic
- **Task**: Play the `block` animation group when the block key is held.
- **File**: `apps/openmw/mwmechanics/character.cpp`.
- **Constraint**: Needs to prevent attacking while blocking (or allow it with a penalty).

## 3. Damage Mitigation
- **Logic**: When in the "Block" state, incoming physical damage should be significantly reduced (e.g., -75%) and diverted to Stamina.
- **File**: `apps/openmw/mwmechanics/creaturestats.cpp` (in `takeDamage`).

## 4. AI Integration
- **Task**: Update `AiCombat` to allow NPCs to "Hold Block" when the player is preparing an attack.
- **File**: `apps/openmw/mwmechanics/aicombat.cpp`.

## 5. Visual Feedback
- Add a stagger or a specific "clink" sound when a hit is blocked while active.
