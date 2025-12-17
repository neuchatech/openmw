# Sprint 6 Onboarding: Hit Rate & Health Regen

Welcome to the OpenMW codebase! This document outlines the current state of the project and provides instructions for the Sprint 6 tasks.

## 1. Project Context
OpenMW is an open-source open-world RPG engine. The codebase is divided into:
-   **`apps/openmw`**: The main game engine (C++).
-   **`apps/opencs`**: The editor (Construction Set).
-   **`components`**: Shared libraries.

Use [`codebase_structure.md`](codebase_structure.md) for a specific map of features to files.

## 2. Current State
*   **Recent Change**: Player movement speed has been increased by 50% in `apps/openmw/mwphysics/movementsolver.cpp`.
*   **Build System**: CMake + Ninja (Visual Studio 2022 environment).
*   **Latest Compilation**: Successfully restored and verified.

## 3. How to Build
We use a **Ninja** build system in the `MSVC2022_64_Ninja` directory.

**Powershell / CMD:**
```powershell
d:
cd d:\data\code\openmw
. ./ActivateMSVC.ps1
cmake --build MSVC2022_64_Ninja --target openmw --config Release
```

**Note**: If the build environment is ever lost, use the following command in **Git Bash** to restore it:
```bash
./CI/before_script.msvc.sh -k -p Win64 -v 2022 -N
```

## 4. Next Tasks: Hit-Rate & Health Regen

Your goal is to "improve hit-rate and tweak enemy health regen". Here are the files you will likely need:

### A. Hit Rate
*   **File**: [`apps/openmw/mwmechanics/combat.cpp`](../apps/openmw/mwmechanics/combat.cpp)
*   **Key Functions**:
    *   `getHitChance()`: Calculates the probability of a hit. Look for `attackTerm` and `defenseTerm`.
    *   `projectileHit()`: Handles ranged attacks.

### B. Enemy Health Regen
*   **File**: [`apps/openmw/mwmechanics/creaturestats.cpp`](../apps/openmw/mwmechanics/creaturestats.cpp)
*   **Key Functions**:
    *   `CreatureStats::takeDamage()`: Logic for applying damage.
    *   `CreatureStats` class generally manages health/fatigue/magicka.
*   **File**: [`apps/openmw/mwmechanics/mechanicsmanagerimp.cpp`](../apps/openmw/mwmechanics/mechanicsmanagerimp.cpp)
    *   Look for the main update loop (often `update()`) where regeneration ticks might occur if not in `Actor`.

**Tip**: Always use the **"Feature Map"** in [`codebase_structure.md`](codebase_structure.md) to find other related files!
