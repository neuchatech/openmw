---
description: Sprint 7 - Guaranteed Hits & Damage Scaling
---

# Sprint 7: Guaranteed Hits Workflow

This sprint focuses on removing "dice roll" misses from combat. Instead of missing, every swing (that physically connects) will deal damage, but the amount will be scaled by the character's skill and attributes (the old "Hit Chance").

## 1. Disable Hit Chance Dice Roll
- **Location**: `apps/openmw/mwmechanics/combat.cpp`
- **Action**: In `projectileHit` and melee hit calculations, replace the RNG check `Misc::Rng::roll0to99(...) >= getHitChance(...)` with a guaranteed success (or a very high threshold like 95-100%).

## 2. Implement Scaling Damage
- **Logic**: Use the `hitChance` (0-100) to determine a damage multiplier.
- **Formula**: `finalDamage = baseDamage * (random(1, hitChance) / 100.f)`
- **Goal**: A high-skill character deals consistent high damage. A low-skill character deals sporadic, mostly low damage, representing "grazing hits" instead of misses.

## 3. Verify Combat Flow
- Ensure that fast-attacking weapons (Daggers) don't become overpowered due to 100% uptime.
- Check that fatigue still correctly penalizes damage (since it affects `hitChance`).
